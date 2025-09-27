#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"

// --- internal state ---
static const char *src;
static int pos;
static int line;
static int col;

// portable strndup replacement
static char *my_strndup(const char *s, size_t n) {
    char *p = (char*)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

void init_lexer(const char *source) {
    src = source;
    pos = 0;
    line = 1;
    col = 1;
}

static char peek() {
    return src[pos];
}

static char advance() {
    char c = src[pos++];
    if (c == '\n') { line++; col = 1; }
    else col++;
    return c;
}

static int match(char expected) {
    if (peek() == expected) {
        advance();
        return 1;
    }
    return 0;
}

static Token make_token(TokenType type, const char *start, int length) {
    Token t;
    t.type = type;
    t.lexeme = (char*)malloc(length + 1);
    memcpy(t.lexeme, start, length);
    t.lexeme[length] = '\0';
    t.line = line;
    t.column = col - length;
    return t;
}

void free_token(Token *tok) {
    if (tok && tok->lexeme) {
        free(tok->lexeme);
        tok->lexeme = NULL;
    }
}

// --- keyword check ---
static TokenType keyword_type(const char *lex) {
    if (strcmp(lex,"func")==0) return TOKEN_FUNC;
    if (strcmp(lex,"return")==0) return TOKEN_RETURN;
    if (strcmp(lex,"success")==0) return TOKEN_SUCCESS;
    if (strcmp(lex,"failure")==0) return TOKEN_FAILURE;
    if (strcmp(lex,"if")==0) return TOKEN_IF;
    if (strcmp(lex,"else")==0) return TOKEN_ELSE;
    if (strcmp(lex,"while")==0) return TOKEN_WHILE;
    if (strcmp(lex,"for")==0) return TOKEN_FOR;
    if (strcmp(lex,"#include")==0) return TOKEN_INCLUDE;
    return TOKEN_IDENTIFIER;
}

// --- main lexer ---
Token next_token(void) {
    // skip whitespace
    while (isspace(peek())) advance();

    const char *start = src + pos;
    char c = peek();

    if (c == '\0') return make_token(TOKEN_EOF, "", 0);

    // identifiers / keywords
    if (isalpha(c) || c == '_' || c == '#') {
        advance();
        // FIX: do NOT allow '.' inside identifiers
        while (isalnum(peek()) || peek()=='_') advance();
        int len = (src+pos) - start;
        char *lex = my_strndup(start, len);
        TokenType type = keyword_type(lex);
        Token t = make_token(type, start, len);
        free(lex);
        return t;
    }

    // numbers
    if (isdigit(c)) {
        advance();
        while (isdigit(peek())) advance();
        return make_token(TOKEN_INT_LITERAL, start, (src+pos)-start);
    }

    // string literal
    if (c == '"') {
        advance(); // opening "
        while (peek() != '"' && peek() != '\0') advance();
        if (peek() == '"') advance(); // closing "
        return make_token(TOKEN_STRING_LITERAL, start, (src+pos)-start);
    }

    // char literal
    if (c == '\'') {
        advance(); // opening '
        while (peek() != '\'' && peek() != '\0') advance();
        if (peek() == '\'') advance(); // closing '
        return make_token(TOKEN_CHAR_LITERAL, start, (src+pos)-start);
    }

    // operators and symbols
    switch (advance()) {
        case '{': return make_token(TOKEN_LBRACE, start, 1);
        case '}': return make_token(TOKEN_RBRACE, start, 1);
        case '(': return make_token(TOKEN_LPAREN, start, 1);
        case ')': return make_token(TOKEN_RPAREN, start, 1);
        case ';': return make_token(TOKEN_SEMICOLON, start, 1);
        case ',': return make_token(TOKEN_COMMA, start, 1);
        case '.': return make_token(TOKEN_DOT, start, 1);
        case '+': return make_token(TOKEN_PLUS, start, 1);
        case '-': return make_token(TOKEN_MINUS, start, 1);
        case '*': return make_token(TOKEN_STAR, start, 1);
        case '/': return make_token(TOKEN_SLASH, start, 1);
        case '=':
            if (match('=')) return make_token(TOKEN_EQ, start, 2);
            return make_token(TOKEN_ASSIGN, start, 1);
        case '!':
            if (match('=')) return make_token(TOKEN_NEQ, start, 2);
            break;
        case '>':
            if (match('=')) return make_token(TOKEN_GTE, start, 2);
            return make_token(TOKEN_GT, start, 1);
        case '<':
            if (match('=')) return make_token(TOKEN_LTE, start, 2);
            return make_token(TOKEN_LT, start, 1);
    }

    // unknown
    return make_token(TOKEN_UNKNOWN, start, 1);
}
