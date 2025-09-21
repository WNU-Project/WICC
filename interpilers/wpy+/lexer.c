#include <stdio.h>
#include <stdlib.h>
#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// -----------------------------
// Lexer state (private to lexer.c)
// -----------------------------
static const char *source = NULL;
static int position = 0;
static int line = 1;

// -----------------------------
// Internal helpers
// -----------------------------
static int is_at_end(void) {
    return !source || source[position] == '\0';
}

static char peek(void) {
    return is_at_end() ? '\0' : source[position];
}

static char peek_next(void) {
    return (is_at_end() || source[position + 1] == '\0') ? '\0' : source[position + 1];
}

static char advance(void) {
    return is_at_end() ? '\0' : source[position++];
}

static char *strndup_local(const char *s, size_t n) {
    size_t len = strlen(s);
    if (len > n) len = n;
    char *result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, s, len);
    result[len] = '\0';
    return result;
}

static Token make_token(TokenType type, const char *lexeme) {
    Token token;
    token.type = type;
    token.lexeme = strndup_local(lexeme, strlen(lexeme));
    token.line = line;
    return token;
}
// Keyword checker
static int is_keyword(const char *lexeme, TokenType *out_type) {
    if (strcmp(lexeme, "func") == 0)    { *out_type = TOKEN_FUNC; return 1; }
    if (strcmp(lexeme, "return") == 0)  { *out_type = TOKEN_RETURN; return 1; }
    if (strcmp(lexeme, "let") == 0)     { *out_type = TOKEN_LET; return 1; }
    if (strcmp(lexeme, "if") == 0)      { *out_type = TOKEN_IF; return 1; }
    if (strcmp(lexeme, "else") == 0)    { *out_type = TOKEN_ELSE; return 1; }
    if (strcmp(lexeme, "for") == 0)     { *out_type = TOKEN_FOR; return 1; }
    if (strcmp(lexeme, "while") == 0)   { *out_type = TOKEN_WHILE; return 1; }
    if (strcmp(lexeme, "use") == 0)     { *out_type = TOKEN_USE; return 1; }
    if (strcmp(lexeme, "const") == 0)   { *out_type = TOKEN_CONST; return 1; }
    if (strcmp(lexeme, "end") == 0)     { *out_type = TOKEN_END; return 1; }
    return 0;
}

// -----------------------------
// Public API
// -----------------------------
char *load_file(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        perror("Failed to open source file");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);
    return buffer;
}

void set_source(const char *src) {
    source = src;
    position = 0;
    line = 1;
}

// -----------------------------
// Tokenizer
// -----------------------------
Token next_token(void) {
    // Skip whitespace
    while (!is_at_end() && isspace(peek())) {
        if (peek() == '\n') line++;
        advance();
    }

    if (is_at_end()) return make_token(TOKEN_EOF, "EOF");

    char c = advance();

    // Identifiers / keywords
 if (isalpha(c) || c == '_') {
    int start = position - 1;
    while (isalnum(peek()) || peek() == '_') advance();
    int length = position - start;
    char *lexeme = strndup_local(source + start, length);

    TokenType type = TOKEN_IDENTIFIER;
    if (is_keyword(lexeme, &type)) {
        return make_token(type, lexeme);
    }
    return make_token(TOKEN_IDENTIFIER, lexeme);
 }


    // Numbers
    if (isdigit(c)) {
        int start = position - 1;
        while (isdigit(peek())) advance();
        int length = position - start;
        char *lexeme = strndup_local(source + start, length);
        return make_token(TOKEN_NUMBER, lexeme);
    }

    // Strings
    if (c == '"') {
        int start = position;
        while (peek() != '"' && !is_at_end()) {
            if (peek() == '\n') line++;
            advance();
        }
        int length = position - start;
        char *lexeme = strndup_local(source + start, length);
        if (peek() == '"') advance(); // consume closing quote
        return make_token(TOKEN_STRING, lexeme);
    }

// Preprocessor directives (#include <pypstdio>)
if (c == '#') {
    int start = position;
    while (isalpha(peek())) advance();
    int length = position - start;
    char *word = strndup_local(source + start, length);

    if (strcmp(word, "include") == 0) {
        if (peek() == '<') {
            advance();
            int fname_start = position;
            while (peek() != '>' && !is_at_end()) advance();
            int fname_len = position - fname_start;
            char *fname = strndup_local(source + fname_start, fname_len);
            if (peek() == '>') advance();

            // Collapse into one token
            return make_token(TOKEN_INCLUDE, fname);
        }
    }

    // Skip unknown preprocessor directives
    while (!is_at_end() && peek() != '\n') advance();
    return next_token();
}




    // Comments
    if (c == '/') {
        if (peek() == '/') {
            while (peek() != '\n' && !is_at_end()) advance();
            return next_token(); // skip comment
        } else if (peek() == '*') {
            advance(); // consume '*'
            while (!(peek() == '*' && peek_next() == '/') && !is_at_end()) {
                if (peek() == '\n') line++;
                advance();
            }
            if (!is_at_end()) { advance(); advance(); } // consume */
            return next_token(); // skip comment
        }
        return make_token(TOKEN_SLASH, "/");
    }

    // Single-character tokens
    switch (c) {
        case '(': return make_token(TOKEN_LPAREN, "(");
        case ')': return make_token(TOKEN_RPAREN, ")");
        case '{': return make_token(TOKEN_LBRACE, "{");
        case '}': return make_token(TOKEN_RBRACE, "}");
        case ';': return make_token(TOKEN_SEMICOLON, ";");
        case '+': return make_token(TOKEN_PLUS, "+");
        case '-': return make_token(TOKEN_MINUS, "-");
        case '*': return make_token(TOKEN_STAR, "*");
        case '<': return make_token(TOKEN_LT, "<");
        case '>': return make_token(TOKEN_GT, ">");
        case '=':
            if (peek() == '=') { advance(); return make_token(TOKEN_EQEQ, "=="); }
            return make_token(TOKEN_EQUAL, "=");
        case '!':
            if (peek() == '=') { advance(); return make_token(TOKEN_BANGEQ, "!="); }
            break;
        case '.': return make_token(TOKEN_DOT, ".");
        case ',': return make_token(TOKEN_COMMA, ",");
    }

    // Unknown character
    fprintf(stderr, "Unexpected character '%c' at line %d\n", c, line);
    return make_token(TOKEN_IDENTIFIER, "?");
}
