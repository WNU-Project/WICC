#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// -----------------------------
// Lexer state
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
    return (is_at_end() || source[position+1] == '\0') ? '\0' : source[position+1];
}

static char advance(void) {
    return is_at_end() ? '\0' : source[position++];
}

static char *strdup_local(const char *s) {
    size_t n = strlen(s);
    char *result = malloc(n+1);
    if (!result) return NULL;
    memcpy(result, s, n);
    result[n] = '\0';
    return result;
}

static char *strndup_local(const char *s, size_t n) {
    char *result = malloc(n+1);
    if (!result) return NULL;
    memcpy(result, s, n);
    result[n] = '\0';
    return result;
}

static Token make_token(TokenType type, const char *lexeme) {
    Token t;
    t.type = type;
    t.lexeme = strdup_local(lexeme);
    t.line = line;
    return t;
}

// -----------------------------
// Public API
// -----------------------------
void set_source(const char *src) {
    source = src;
    position = 0;
    line = 1;

    // Strip UTF-8 BOM if present
    if (source &&
        (unsigned char)source[0] == 0xEF &&
        (unsigned char)source[1] == 0xBB &&
        (unsigned char)source[2] == 0xBF) {
        position = 3;
    }
}

char *load_file(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) { perror("open"); exit(1); }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    char *buf = malloc(size+1);
    fread(buf,1,size,fp);
    buf[size] = '\0';
    fclose(fp);
    return buf;
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

    // Comments
    if (c == '/') {
        if (peek() == '/') {
            while (peek() != '\n' && !is_at_end()) advance();
            return next_token();
        } else if (peek() == '*') {
            advance();
            while (!(peek() == '*' && peek_next() == '/') && !is_at_end()) {
                if (peek() == '\n') line++;
                advance();
            }
            if (!is_at_end()) { advance(); advance(); }
            return next_token();
        }
        return make_token(TOKEN_SLASH, "/");
    }

    // Identifiers / keywords / types
    if (isalpha(c) || c == '_') {
        int start = position - 1;
        while (isalnum(peek()) || peek() == '_') advance();
        int len = position - start;
        char *lex = strndup_local(source + start, len);

        // keywords
        if (strcmp(lex,"func")==0) return make_token(TOKEN_FUNC,lex);
        if (strcmp(lex,"return")==0) return make_token(TOKEN_RETURN,lex);
        if (strcmp(lex,"if")==0) return make_token(TOKEN_IF,lex);
        if (strcmp(lex,"else")==0) return make_token(TOKEN_ELSE,lex);
        if (strcmp(lex,"for")==0) return make_token(TOKEN_FOR,lex);
        if (strcmp(lex,"while")==0) return make_token(TOKEN_WHILE,lex);
        if (strcmp(lex,"use")==0) return make_token(TOKEN_USE,lex);
        if (strcmp(lex,"end")==0) return make_token(TOKEN_END,lex);

        // types
        if (strcmp(lex,"int")==0) return make_token(TOKEN_TYPE_INT,lex);
        if (strcmp(lex,"char")==0) return make_token(TOKEN_TYPE_CHAR,lex);
        if (strcmp(lex,"string")==0) return make_token(TOKEN_TYPE_CHAR_STRING,lex);
        if (strcmp(lex,"float")==0) return make_token(TOKEN_TYPE_FLOAT,lex);
        if (strcmp(lex,"bool")==0) return make_token(TOKEN_TYPE_BOOL,lex);

        return make_token(TOKEN_IDENTIFIER, lex);
    }

    // Numbers
    if (isdigit(c)) {
        int start = position - 1;
        while (isdigit(peek())) advance();
        int len = position - start;
        char *lex = strndup_local(source + start, len);
        return make_token(TOKEN_NUMBER, lex);
    }

    // Strings
    if (c == '"') {
        int start = position;
        while (peek() != '"' && !is_at_end()) {
            if (peek() == '\n') line++;
            advance();
        }
        int len = position - start;
        char *lex = strndup_local(source + start, len);
        if (peek() == '"') advance();
        return make_token(TOKEN_STRING, lex);
    }

    // Character literal
    if (c == '\'') {
        char ch = advance();
        char buf[2] = { ch, '\0' };
        if (peek() == '\'') advance(); // consume closing '
        return make_token(TOKEN_CHAR_LITERAL, buf);
    }

    // Preprocessor directives
    if (c == '#') {
        int start = position;
        while (isalpha(peek())) advance();
        int len = position - start;
        char *word = strndup_local(source + start, len);

        if (word && strcmp(word, "include") == 0) {
            while (isspace(peek())) advance();
            if (peek() == '<') {
                advance();
                int fname_start = position;
                while (!is_at_end() && peek() != '>') advance();
                int fname_len = position - fname_start;
                char *fname = strndup_local(source + fname_start, fname_len);
                if (peek() == '>') advance();
                return make_token(TOKEN_INCLUDE, fname ? fname : "");
            }
        }

        if (peek() == '<') {
            advance();
            int fname_start = position;
            while (!is_at_end() && peek() != '>') advance();
            int fname_len = position - fname_start;
            char *fname = strndup_local(source + fname_start, fname_len);
            if (peek() == '>') advance();
            return make_token(TOKEN_INCLUDE, fname ? fname : "");
        }

        while (!is_at_end() && peek() != '\n') advance();
        return next_token();
    }

    // Single-character tokens
    switch (c) {
        case '(': return make_token(TOKEN_LPAREN,"(");
        case ')': return make_token(TOKEN_RPAREN,")");
        case '{': return make_token(TOKEN_LBRACE,"{");
        case '}': return make_token(TOKEN_RBRACE,"}");
        case ';': return make_token(TOKEN_SEMICOLON,";");
        case '+': return make_token(TOKEN_PLUS,"+");
        case '-': return make_token(TOKEN_MINUS,"-");
        case '*': return make_token(TOKEN_STAR,"*");
        case '<': return make_token(TOKEN_LT,"<");
        case '>': return make_token(TOKEN_GT,">");
        case '=':
            if (peek() == '=') { advance(); return make_token(TOKEN_EQEQ,"=="); }
            return make_token(TOKEN_EQUAL,"=");
        case '!':
            if (peek() == '=') { advance(); return make_token(TOKEN_BANGEQ,"!="); }
            break;
        case '.': return make_token(TOKEN_DOT,".");
        case ',': return make_token(TOKEN_COMMA,",");
    }

    // Unknown
    fprintf(stderr,"Unexpected char '%c' at line %d\n", c, line);
    return make_token(TOKEN_IDENTIFIER, "?");
}

int lex_line(const char *line, Token *tokens) {
    set_source(line);
    int count = 0;
    Token tok;
    do {
        tok = next_token();
        tokens[count++] = tok;
    } while (tok.type != TOKEN_EOF && count < 256);
    return count;
}
