#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"

// -----------------------------
// Local helpers
// -----------------------------

// Windows-safe replacement for strndup
static char *strndup_local(const char *s, size_t n) {
    size_t len = strlen(s);
    if (len > n) len = n;
    char *result = (char *)malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, s, len);
    result[len] = '\0';
    return result;
}

// -----------------------------
// Lexer state
// -----------------------------
static const char *source = NULL;
static int position = 0;
static int line = 1;

// Forward declarations
Token next_token(void);
static Token make_token(TokenType type, const char *lexeme);
static int is_keyword(const char *lexeme, TokenType *out_type);

// -----------------------------
// File loader
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

// -----------------------------
// Source initializer
// -----------------------------
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
    while (source && isspace(source[position])) {
        if (source[position] == '\n') line++;
        position++;
    }

    if (!source || source[position] == '\0') {
        return make_token(TOKEN_EOF, "EOF");
    }

    char c = source[position];

    // Identifiers / keywords
    if (isalpha(c) || c == '_') {
        int start = position;
        while (isalnum(source[position]) || source[position] == '_') {
            position++;
        }
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
        int start = position;
        while (isdigit(source[position])) position++;
        int length = position - start;
        char *lexeme = strndup_local(source + start, length);
        return make_token(TOKEN_NUMBER, lexeme);
    }

    // Strings
    if (c == '"') {
        position++; // skip opening quote
        int start = position;
        while (source[position] != '"' && source[position] != '\0') {
            if (source[position] == '\n') line++;
            position++;
        }
        int length = position - start;
        char *lexeme = strndup_local(source + start, length);
        if (source[position] == '"') position++; // skip closing quote
        return make_token(TOKEN_STRING, lexeme);
    }

    // Preprocessor / directives starting with '#'
    if (c == '#') {
     position++;
     int start = position;
     while (isalnum(source[position])) position++;
     int length = position - start;
     char *lexeme = strndup_local(source + start, length);
     if (strcmp(lexeme, "include") == 0) {
        return make_token(TOKEN_INCLUDE, lexeme);
     }
     return make_token(TOKEN_HASH, lexeme);
    }

    // Single-character tokens
    switch (c) {
        case '(': position++; return make_token(TOKEN_LPAREN, "(");
        case ')': position++; return make_token(TOKEN_RPAREN, ")");
        case '{': position++; return make_token(TOKEN_LBRACE, "{");
        case '}': position++; return make_token(TOKEN_RBRACE, "}");
        case ';': position++; return make_token(TOKEN_SEMICOLON, ";");
        case '.': position++; return make_token(TOKEN_DOT, ".");
        case '+': position++; return make_token(TOKEN_PLUS, "+");
        case '-': position++; return make_token(TOKEN_MINUS, "-");
        case '*': position++; return make_token(TOKEN_STAR, "*");
        case '/': position++; return make_token(TOKEN_SLASH, "/");
        case '<': position++; return make_token(TOKEN_LT, "<");
        case '>': position++; return make_token(TOKEN_GT, ">");
        case '#':
            if (source[position+1] == 'i') {
                position += 2;
                return make_token(TOKEN_INCLUDE, "#include");
            }
            return make_token(TOKEN_HASH, "#");
        case '=':
            if (source[position+1] == '=') {
                position += 2;
                return make_token(TOKEN_EQEQ, "==");
            }
            position++;
            return make_token(TOKEN_EQUAL, "=");
        case '!':
            if (source[position+1] == '=') {
                position += 2;
                return make_token(TOKEN_BANGEQ, "!=");
            }
            break;
    }

    // Unknown character
    fprintf(stderr, "Unexpected character '%c' at line %d\n", c, line);
    position++;
    return make_token(TOKEN_IDENTIFIER, "?");
}

// -----------------------------
// Helpers
// -----------------------------
static Token make_token(TokenType type, const char *lexeme) {
    Token token;
    token.type = type;
    token.lexeme = strdup(lexeme);
    token.line = line;
    return token;
}

static int is_keyword(const char *lexeme, TokenType *out_type) {
    if (strcmp(lexeme, "func") == 0) { *out_type = TOKEN_FUNC; return 1; }
    if (strcmp(lexeme, "return") == 0) { *out_type = TOKEN_RETURN; return 1; }
    if (strcmp(lexeme, "let") == 0) { *out_type = TOKEN_LET; return 1; }
    if (strcmp(lexeme, "if") == 0) { *out_type = TOKEN_IF; return 1; }
    if (strcmp(lexeme, "else") == 0) { *out_type = TOKEN_ELSE; return 1; }
    if (strcmp(lexeme, "for") == 0) { *out_type = TOKEN_FOR; return 1; }
    if (strcmp(lexeme, "while") == 0) { *out_type = TOKEN_WHILE; return 1; }
    if (strcmp(lexeme, "use") == 0) { *out_type = TOKEN_USE; return 1; }
    if (strcmp(lexeme, "const") == 0) { *out_type = TOKEN_CONST; return 1; }
    if (strcmp(lexeme, "end") == 0) { *out_type = TOKEN_END; return 1; }
    return 0;
}
