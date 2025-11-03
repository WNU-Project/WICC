#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "tokens.h"
#include "ast.h"

// Simple recursive-descent parser for a small subset:
// translation-unit := { function-definition }
// function-definition := 'int' identifier '(' ')' compound-stmt
// compound-stmt := '{' { statement } '}'
// statement := return-stmt | decl-stmt | expr-stmt
// return-stmt := 'return' expression ';'
// decl-stmt := 'int' identifier [ '=' expression ] ';'
// expr-stmt := expression ';'
// expression := integer-literal | identifier

static Token cur;
static int have_cur = 0;

static Token next_token(void) {
    if (have_cur) { have_cur = 0; return cur; }
    return lexer_next();
}

static void push_token(Token t) {
    cur = t;
    have_cur = 1;
}

static Token peek_token(void) {
    if (!have_cur) {
        cur = lexer_next();
        have_cur = 1;
    }
    return cur;
}

static int accept_kind(TokenKind k, Token *out) {
    Token t = peek_token();
    if (t.kind == k) {
        next_token();
        if (out) *out = t;
        return 1;
    }
    return 0;
}

static int expect_kind(TokenKind k, Token *out) {
    if (accept_kind(k, out)) return 1;
    Token t = peek_token();
    fprintf(stderr, "parser error: expected token kind %d at %d:%d\n", (int)k, t.line, t.column);
    return 0;
}

// helpers to create nodes
static Ast *make_ident_from_token(Token t) {
    Ast *a = ast_new(AST_IDENTIFIER);
    a->lexeme = t.lexeme ? strdup(t.lexeme) : NULL;
    return a;
}

static Ast *parse_expression(void) {
    Token t = peek_token();
    if (t.kind == TOK_INT_LITERAL) {
        next_token();
        Ast *n = ast_new(AST_INT_LITERAL);
        n->int_value = atoi(t.lexeme);
        if (t.lexeme) free((void*)t.lexeme);
        return n;
    }
    if (t.kind == TOK_STRING_LITERAL) {
        next_token();
        Ast *n = ast_new(AST_STRING_LITERAL);
        n->lexeme = t.lexeme ? strdup(t.lexeme) : NULL;
        if (t.lexeme) free((void*)t.lexeme);
        return n;
    }
    if (t.kind == TOK_IDENTIFIER) {
        // Consume identifier token
        next_token();
        char *name = t.lexeme ? strdup(t.lexeme) : NULL;
        if (t.lexeme) free((void*)t.lexeme);

        // If followed by '(', this is a call expression
        if (accept_kind(TOK_LPAREN, NULL)) {
            Ast *call = ast_new(AST_CALL_EXPR);
            // callee as first child
            Ast *callee = ast_new(AST_IDENTIFIER);
            callee->lexeme = name ? strdup(name) : NULL;
            ast_append_child(call, callee);

            // parse argument list (zero or more expressions separated by comma)
            Token p = peek_token();
            if (p.kind != TOK_RPAREN) {
                for (;;) {
                    Ast *arg = parse_expression();
                    ast_append_child(call, arg);
                    if (accept_kind(TOK_COMMA, NULL)) continue;
                    break;
                }
            }
            // expect closing ')'
            accept_kind(TOK_RPAREN, NULL);
            if (name) free(name);
            return call;
        }

        // Not a call: simple identifier
        Ast *n = ast_new(AST_IDENTIFIER);
        n->lexeme = name;
        return n;
    }
    // Unknown expression
    next_token();
    if (t.lexeme) free((void*)t.lexeme);
    return ast_new(AST_UNKNOWN);
}

static Ast *parse_statement(void) {
    Token t = peek_token();
    if (t.kind == TOK_KW_RETURN) {
        next_token();
        Ast *ret = ast_new(AST_RETURN_STMT);
        Ast *expr = parse_expression();
        ast_append_child(ret, expr);
        accept_kind(TOK_SEMICOLON, NULL);
        return ret;
    }
    if (t.kind == TOK_KW_INT) {
        // declaration
        next_token();
        Token id;
        if (!expect_kind(TOK_IDENTIFIER, &id)) return NULL;
        Ast *decl = ast_new(AST_DECL_STMT);
        decl->name = id.lexeme ? strdup(id.lexeme) : NULL;
        // optional initializer
        if (accept_kind(TOK_ASSIGN, NULL)) {
            Ast *rhs = parse_expression();
            ast_append_child(decl, rhs);
        }
        accept_kind(TOK_SEMICOLON, NULL);
        return decl;
    }
    // expression statement
    Ast *expr = parse_expression();
    accept_kind(TOK_SEMICOLON, NULL);
    Ast *stmt = ast_new(AST_EXPR_STMT);
    ast_append_child(stmt, expr);
    return stmt;
}

static Ast *parse_compound_stmt(void) {
    Ast *c = ast_new(AST_COMPOUND_STMT);
    if (!accept_kind(TOK_LBRACE, NULL)) return c;
    while (!accept_kind(TOK_RBRACE, NULL)) {
        Token p = peek_token();
        if (p.kind == TOK_EOF) break;
        Ast *st = parse_statement();
        if (!st) break;
        ast_append_child(c, st);
    }
    return c;
}

static Ast *parse_function_definition(void) {
    // expect 'int' IDENT '(' ')' compound
    Token t;
    if (!expect_kind(TOK_KW_INT, NULL)) return NULL;
    if (!expect_kind(TOK_IDENTIFIER, &t)) return NULL;
    Ast *fn = ast_new(AST_FUNC_DEF);
    fn->name = t.lexeme ? strdup(t.lexeme) : NULL;
    // params: currently expect '()'
    accept_kind(TOK_LPAREN, NULL);
    accept_kind(TOK_RPAREN, NULL);
    Ast *body = parse_compound_stmt();
    ast_append_child(fn, body);
    return fn;
}

Ast *parser_parse_translation_unit(void) {
    Ast *tu = ast_new(AST_TRANSLATION_UNIT);
    for (;;) {
        Token p = peek_token();
        if (p.kind == TOK_EOF) break;
        // Only handle simple function defs starting with 'int'
        if (p.kind == TOK_KW_INT) {
            Ast *fd = parse_function_definition();
            if (!fd) break;
            ast_append_child(tu, fd);
            continue;
        }
        // consume unknown token to avoid infinite loop
        Token drop = next_token();
        if (drop.lexeme) free((void*)drop.lexeme);
        if (drop.kind == TOK_EOF) break;
    }
    return tu;
}

int parser_init(const char *path) {
    if (lexer_init(path) != 0) return -1;
    have_cur = 0;
    return 0;
}

void parser_free(void) {
    lexer_free();
}
