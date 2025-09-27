#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static Token *tokens;
static int pos;
static int count;

// portable strdup replacement
static char *my_strdup(const char *s) {
    size_t n = strlen(s);
    char *p = (char*)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n + 1);
    return p;
}

static Token *peek() { return (pos < count) ? &tokens[pos] : NULL; }
static Token *advance() { return (pos < count) ? &tokens[pos++] : NULL; }
static int match(TokenType type) {
    if (peek() && peek()->type == type) { advance(); return 1; }
    return 0;
}

static ASTNode *make_node(ASTNodeType type, const char *value) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = type;
    n->value = value ? my_strdup(value) : NULL;
    n->children = NULL;
    n->child_count = 0;
    return n;
}

static void add_child(ASTNode *parent, ASTNode *child) {
    if (!child) return;
    parent->children = realloc(parent->children, sizeof(ASTNode*) * (parent->child_count+1));
    parent->children[parent->child_count++] = child;
}

// --- parsing functions ---

static ASTNode *parse_literal_or_identifier() {
    Token *t = advance();
    if (!t) return NULL;
    if (t->type == TOKEN_STRING_LITERAL || t->type == TOKEN_INT_LITERAL || t->type == TOKEN_CHAR_LITERAL) {
        return make_node(AST_LITERAL, t->lexeme);
    }
    if (t->type == TOKEN_IDENTIFIER) {
        return make_node(AST_IDENTIFIER, t->lexeme);
    }
    return NULL;
}

static ASTNode *parse_call() {
    int save = pos;

    if (!match(TOKEN_IDENTIFIER)) { pos = save; return NULL; }
    Token *module = &tokens[pos-1];

    if (!match(TOKEN_DOT) || !match(TOKEN_IDENTIFIER)) { pos = save; return NULL; }
    Token *func = &tokens[pos-1];

    if (!match(TOKEN_LPAREN)) { pos = save; return NULL; }

    // Build full callee name
    size_t name_len = strlen(module->lexeme) + 1 + strlen(func->lexeme) + 1;
    char *callee = (char*)malloc(name_len);
    snprintf(callee, name_len, "%s.%s", module->lexeme, func->lexeme);

    // Special case: pyppstdio.print → AST_PRINT
    ASTNodeType type = (strcmp(module->lexeme,"pyppstdio")==0 && strcmp(func->lexeme,"print")==0)
                       ? AST_PRINT : AST_IDENTIFIER;

    ASTNode *callNode = make_node(type, callee);
    free(callee);

    // arguments
    while (peek() && peek()->type != TOKEN_RPAREN) {
        ASTNode *arg = parse_literal_or_identifier();
        if (arg) add_child(callNode, arg);
        if (peek() && peek()->type == TOKEN_COMMA) advance();
        else if (peek() && peek()->type != TOKEN_RPAREN) {
            free_ast(callNode);
            pos = save;
            return NULL;
        }
    }

    if (!match(TOKEN_RPAREN) || !match(TOKEN_SEMICOLON)) {
        free_ast(callNode);
        pos = save;
        return NULL;
    }

    return callNode;
}

static ASTNode *parse_return() {
    ASTNode *ret = make_node(AST_RETURN, NULL);
    advance(); // consume 'return'

    if (peek()) {
        if (peek()->type == TOKEN_SUCCESS || peek()->type == TOKEN_FAILURE || peek()->type == TOKEN_INT_LITERAL) {
            ASTNode *val = make_node(AST_LITERAL, peek()->lexeme);
            add_child(ret, val);
            advance();
        }
    }

    match(TOKEN_SEMICOLON);
    return ret;
}

static ASTNode *parse_statement() {
    if (!peek()) return NULL;
    if (peek()->type == TOKEN_RETURN) return parse_return();

    int save = pos;
    ASTNode *c = parse_call();
    if (c) return c;
    pos = save;

    advance();
    return NULL;
}

static ASTNode *parse_block() {
    ASTNode *block = make_node(AST_BLOCK, NULL);
    match(TOKEN_LBRACE);
    while (peek() && peek()->type != TOKEN_RBRACE) {
        ASTNode *stmt = parse_statement();
        if (stmt) add_child(block, stmt);
    }
    match(TOKEN_RBRACE);
    return block;
}

static ASTNode *parse_function() {
    // Accept either "func" or "void"
    if (match(TOKEN_FUNC) || match(TOKEN_IDENTIFIER)) {
        Token *name = advance(); // function name
        ASTNode *func = make_node(AST_FUNCTION, name ? name->lexeme : NULL);
        match(TOKEN_LPAREN);
        match(TOKEN_RPAREN);
        ASTNode *body = parse_block();
        add_child(func, body);
        return func;
    }
    return NULL;
}

ASTNode *parse(Token *toks, int c) {
    tokens = toks;
    pos = 0;
    count = c;
    ASTNode *program = make_node(AST_PROGRAM, NULL);
    while (peek() && peek()->type != TOKEN_EOF) {
        if (peek()->type == TOKEN_FUNC || (peek()->type == TOKEN_IDENTIFIER && strcmp(peek()->lexeme,"void")==0)) {
            ASTNode *f = parse_function();
            if (f) add_child(program, f);
        } else {
            advance();
        }
    }
    return program;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    for (int i=0; i<node->child_count; i++) {
        free_ast(node->children[i]);
    }
    free(node->children);
    if (node->value) free(node->value);
    free(node);
}

static const char* node_type_name(ASTNodeType t) {
    switch(t) {
        case AST_PROGRAM:    return "PROGRAM";
        case AST_FUNCTION:   return "FUNCTION";
        case AST_BLOCK:      return "BLOCK";
        case AST_PRINT:      return "PRINT";
        case AST_RETURN:     return "RETURN";
        case AST_LITERAL:    return "LITERAL";
        case AST_IDENTIFIER: return "CALL";
        default:             return "UNKNOWN";
    }
}

void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    for (int i=0; i<indent; i++) printf("  ");
    printf("%s", node_type_name(node->type));
    if (node->value) printf(" (%s)", node->value);
    printf("\n");
    for (int i=0; i<node->child_count; i++) {
        print_ast(node->children[i], indent+1);
    }
}
