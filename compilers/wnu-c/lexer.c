#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

// Simple buffer-based lexer implementation for wnu-c.
// Supports identifiers, integer literals, string/char literals,
// common multi-char operators and basic punctuation. This is
// intentionally small and intended as a starting point.

static char *src_buf = NULL;
static size_t src_len = 0;
static size_t src_pos = 0;
static int src_line = 1;
static int src_col = 1;

static char *strdup_local(const char *s, size_t len) {
    char *p = malloc(len + 1);
    if (!p) return NULL;
    memcpy(p, s, len);
    p[len] = '\0';
    return p;
}

int lexer_init(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    src_buf = malloc(sz + 1);
    if (!src_buf) {
        fclose(f);
        return -1;
    }
    size_t r = fread(src_buf, 1, sz, f);
    fclose(f);
    src_buf[r] = '\0';
    src_len = r;
    src_pos = 0;
    src_line = 1;
    src_col = 1;
    return 0;
}

void lexer_free(void) {
    free(src_buf);
    src_buf = NULL;
    src_len = src_pos = 0;
}

static int peek_char(void) {
    if (!src_buf) return EOF;
    if (src_pos >= src_len) return EOF;
    return (unsigned char)src_buf[src_pos];
}

static int get_char(void) {
    int c = peek_char();
    if (c == EOF) return EOF;
    src_pos++;
    if (c == '\n') {
        src_line++;
        src_col = 1;
    } else {
        src_col++;
    }
    return c;
}

static void skip_whitespace_and_comments(void) {
    for (;;) {
        int c = peek_char();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            get_char();
            continue;
        }
        // line comment //
        if (c == '/' && src_pos + 1 < src_len && src_buf[src_pos + 1] == '/') {
            get_char(); // /
            get_char(); // /
            while (peek_char() != EOF && peek_char() != '\n') get_char();
            continue;
        }
        // block comment /* */
        if (c == '/' && src_pos + 1 < src_len && src_buf[src_pos + 1] == '*') {
            get_char(); // /
            get_char(); // *
            while (peek_char() != EOF) {
                if (peek_char() == '*' && src_pos + 1 < src_len && src_buf[src_pos + 1] == '/') {
                    get_char(); get_char();
                    break;
                }
                get_char();
            }
            continue;
        }
        break;
    }
}

static int is_ident_start(int c) {
    return (c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

static int is_ident_part(int c) {
    return is_ident_start(c) || (c >= '0' && c <= '9');
}

Token lexer_next(void) {
    Token t;
    t.kind = TOK_UNKNOWN;
    t.lexeme = NULL;
    t.line = src_line;
    t.column = src_col;

    if (!src_buf) {
        t.kind = TOK_EOF;
        return t;
    }

    skip_whitespace_and_comments();

    int c = peek_char();
    if (c == EOF) {
        t.kind = TOK_EOF;
        return t;
    }

    // Identifiers or keywords
    if (is_ident_start(c)) {
        size_t start = src_pos;
        get_char();
        while (is_ident_part(peek_char())) get_char();
        size_t len = src_pos - start;
        const char *lex = src_buf + start;
        // Copy lexeme
        char *lexeme = strdup_local(lex, len);
        // Keyword recognition: map common keywords to TokenKind
        TokenKind k = TOK_IDENTIFIER;
        if (strcmp(lexeme, "int") == 0) k = TOK_KW_INT;
        else if (strcmp(lexeme, "return") == 0) k = TOK_KW_RETURN;
        else if (strcmp(lexeme, "if") == 0) k = TOK_KW_IF;
        else if (strcmp(lexeme, "else") == 0) k = TOK_KW_ELSE;
        else if (strcmp(lexeme, "while") == 0) k = TOK_KW_WHILE;
        else if (strcmp(lexeme, "for") == 0) k = TOK_KW_FOR;
        else if (strcmp(lexeme, "struct") == 0) k = TOK_KW_STRUCT;
        else if (strcmp(lexeme, "typedef") == 0) k = TOK_KW_TYPEDEF;
        else if (strcmp(lexeme, "extern") == 0) k = TOK_KW_EXTERN;
        else if (strcmp(lexeme, "static") == 0) k = TOK_KW_STATIC;
        else if (strcmp(lexeme, "char") == 0) k = TOK_KW_CHAR;
        else if (strcmp(lexeme, "float") == 0) k = TOK_KW_FLOAT;
        else if (strcmp(lexeme, "double") == 0) k = TOK_KW_DOUBLE;
        else if (strcmp(lexeme, "void") == 0) k = TOK_KW_VOID;
        else if (strcmp(lexeme, "_Bool") == 0) k = TOK_KW__BOOL;

        t.kind = k;
        t.lexeme = lexeme;
        return t;
    }

    // Integer literal (simple decimal)
    if (c >= '0' && c <= '9') {
        size_t start = src_pos;
        get_char();
        while (peek_char() >= '0' && peek_char() <= '9') get_char();
        size_t len = src_pos - start;
        t.kind = TOK_INT_LITERAL;
        t.lexeme = strdup_local(src_buf + start, len);
        return t;
    }

    // String literal
    if (c == '"') {
        size_t start = src_pos;
        get_char(); // consume '"'
        while (peek_char() != EOF) {
            if (peek_char() == '\\') { get_char(); if (peek_char() != EOF) get_char(); continue; }
            if (peek_char() == '"') { get_char(); break; }
            get_char();
        }
        size_t len = src_pos - start;
        t.kind = TOK_STRING_LITERAL;
        t.lexeme = strdup_local(src_buf + start, len);
        return t;
    }

    // Character literal
    if (c == '\'') {
        size_t start = src_pos;
        get_char(); // consume '\''
        if (peek_char() == '\\') { get_char(); if (peek_char() != EOF) get_char(); }
        else if (peek_char() != EOF) get_char();
        if (peek_char() == '\'') get_char();
        size_t len = src_pos - start;
        t.kind = TOK_CHAR_LITERAL;
        t.lexeme = strdup_local(src_buf + start, len);
        return t;
    }

    // Multi-char operators and punctuation
    // Check two-character sequences first
    if (src_pos + 1 <= src_len) {
        char a = src_buf[src_pos];
        char b = (src_pos + 1 < src_len) ? src_buf[src_pos + 1] : '\0';
        // list of two-char tokens
        if (a == '=' && b == '=') { get_char(); get_char(); t.kind = TOK_EQ; t.lexeme = strdup_local("==", 2); return t; }
        if (a == '!' && b == '=') { get_char(); get_char(); t.kind = TOK_NEQ; t.lexeme = strdup_local("!=", 2); return t; }
        if (a == '<' && b == '=') { get_char(); get_char(); t.kind = TOK_LE; t.lexeme = strdup_local("<=", 2); return t; }
        if (a == '>' && b == '=') { get_char(); get_char(); t.kind = TOK_GE; t.lexeme = strdup_local(">=", 2); return t; }
        if (a == '+' && b == '+') { get_char(); get_char(); t.kind = TOK_PLUSPLUS; t.lexeme = strdup_local("++", 2); return t; }
        if (a == '-' && b == '-') { get_char(); get_char(); t.kind = TOK_MINUSMINUS; t.lexeme = strdup_local("--", 2); return t; }
        if (a == '+' && b == '=') { get_char(); get_char(); t.kind = TOK_PLUS_ASSIGN; t.lexeme = strdup_local("+=", 2); return t; }
        if (a == '-' && b == '=') { get_char(); get_char(); t.kind = TOK_MINUS_ASSIGN; t.lexeme = strdup_local("-=", 2); return t; }
        if (a == '-' && b == '>') { get_char(); get_char(); t.kind = TOK_ARROW; t.lexeme = strdup_local("->", 2); return t; }
        if (a == '&' && b == '&') { get_char(); get_char(); t.kind = TOK_AND_AND; t.lexeme = strdup_local("&&", 2); return t; }
        if (a == '|' && b == '|') { get_char(); get_char(); t.kind = TOK_OR_OR; t.lexeme = strdup_local("||", 2); return t; }
        if (a == '-' && b == '>') { get_char(); get_char(); t.kind = TOK_ARROW; t.lexeme = strdup_local("->", 2); return t; }
        if (a == '-' && b == '=') { get_char(); get_char(); t.kind = TOK_MINUS_ASSIGN; t.lexeme = strdup_local("-=", 2); return t; }
        if (a == '<' && b == '<') { get_char(); get_char(); t.kind = TOK_LSHIFT; t.lexeme = strdup_local("<<", 2); return t; }
        if (a == '>' && b == '>') { get_char(); get_char(); t.kind = TOK_RSHIFT; t.lexeme = strdup_local(">>", 2); return t; }
        if (a == '#' && b == '#') { get_char(); get_char(); t.kind = TOK_PP_HASHHASH; t.lexeme = strdup_local("##", 2); return t; }
    }

    // Single-character tokens
    c = get_char();
    switch (c) {
        case '+': t.kind = TOK_PLUS; t.lexeme = strdup_local("+",1); return t;
        case '-': t.kind = TOK_MINUS; t.lexeme = strdup_local("-",1); return t;
        case '*': t.kind = TOK_STAR; t.lexeme = strdup_local("*",1); return t;
        case '/': t.kind = TOK_SLASH; t.lexeme = strdup_local("/",1); return t;
        case '%': t.kind = TOK_PERCENT; t.lexeme = strdup_local("%",1); return t;
        case '=': t.kind = TOK_ASSIGN; t.lexeme = strdup_local("=",1); return t;
        case '<': t.kind = TOK_LT; t.lexeme = strdup_local("<",1); return t;
        case '>': t.kind = TOK_GT; t.lexeme = strdup_local(">",1); return t;
        case '&': t.kind = TOK_AND; t.lexeme = strdup_local("&",1); return t;
        case '|': t.kind = TOK_OR; t.lexeme = strdup_local("|",1); return t;
        case '^': t.kind = TOK_XOR; t.lexeme = strdup_local("^",1); return t;
        case '!': t.kind = TOK_NOT; t.lexeme = strdup_local("!",1); return t;
        case '?': t.kind = TOK_TERNARY; t.lexeme = strdup_local("?",1); return t;
        case ':': t.kind = TOK_COLON; t.lexeme = strdup_local(":",1); return t;
        case ';': t.kind = TOK_SEMICOLON; t.lexeme = strdup_local(";",1); return t;
        case ',': t.kind = TOK_COMMA; t.lexeme = strdup_local(",",1); return t;
        case '.': t.kind = TOK_DOT; t.lexeme = strdup_local(".",1); return t;
        case '(': t.kind = TOK_LPAREN; t.lexeme = strdup_local("(",1); return t;
        case ')': t.kind = TOK_RPAREN; t.lexeme = strdup_local(")",1); return t;
        case '{': t.kind = TOK_LBRACE; t.lexeme = strdup_local("{",1); return t;
        case '}': t.kind = TOK_RBRACE; t.lexeme = strdup_local("}",1); return t;
        case '[': t.kind = TOK_LBRACKET; t.lexeme = strdup_local("[",1); return t;
        case ']': t.kind = TOK_RBRACKET; t.lexeme = strdup_local("]",1); return t;
        case '#': t.kind = TOK_PP_HASH; t.lexeme = strdup_local("#",1); return t;
        default:
            // Unknown single char
            t.kind = TOK_UNKNOWN;
            t.lexeme = strdup_local((const char *)&src_buf[src_pos-1], 1);
            return t;
    }
}