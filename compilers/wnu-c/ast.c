#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Ast *ast_new(AstKind kind) {
    Ast *a = (Ast*)calloc(1, sizeof(Ast));
    if (!a) return NULL;
    a->kind = kind;
    a->next = NULL;
    a->child = NULL;
    a->name = NULL;
    a->lexeme = NULL;
    a->int_value = 0;
    return a;
}

void ast_append_child(Ast *parent, Ast *child) {
    if (!parent || !child) return;
    if (!parent->child) { parent->child = child; return; }
    Ast *p = parent->child;
    while (p->next) p = p->next;
    p->next = child;
}

void ast_free(Ast *a) {
    if (!a) return;
    Ast *c = a->child;
    while (c) {
        Ast *n = c->next;
        ast_free(c);
        c = n;
    }
    if (a->name) free(a->name);
    if (a->lexeme) free(a->lexeme);
    free(a);
}

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) putchar(' ');
}

void ast_print(Ast *a, int indent) {
    if (!a) return;
    print_indent(indent);
    switch (a->kind) {
        case AST_TRANSLATION_UNIT: printf("TranslationUnit\n"); break;
        case AST_FUNC_DEF: printf("Function: %s\n", a->name ? a->name : "(anon)"); break;
        case AST_COMPOUND_STMT: printf("CompoundStmt\n"); break;
        case AST_RETURN_STMT: printf("Return\n"); break;
        case AST_DECL_STMT: printf("Decl: %s\n", a->name ? a->name : ""); break;
        case AST_EXPR_STMT: printf("ExprStmt\n"); break;
    case AST_INT_LITERAL: printf("IntLiteral: %d\n", a->int_value); break;
    case AST_IDENTIFIER: printf("Identifier: %s\n", a->lexeme ? a->lexeme : ""); break;
    case AST_STRING_LITERAL: printf("StringLiteral: %s\n", a->lexeme ? a->lexeme : ""); break;
    case AST_CALL_EXPR: printf("CallExpr\n"); break;
        default: printf("Node kind %d\n", (int)a->kind); break;
    }
    Ast *c = a->child;
    while (c) { ast_print(c, indent + 2); c = c->next; }
}
