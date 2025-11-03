#ifndef AST_H
#define AST_H

#include <stdlib.h>

typedef enum AstKind {
    AST_UNKNOWN = 0,
    AST_TRANSLATION_UNIT,
    AST_FUNC_DEF,
    AST_PARAM_LIST,
    AST_COMPOUND_STMT,
    AST_RETURN_STMT,
    AST_DECL_STMT,
    AST_EXPR_STMT,
    AST_BINARY_EXPR,
    AST_CALL_EXPR,
    AST_STRING_LITERAL,
    AST_INT_LITERAL,
    AST_IDENTIFIER,
} AstKind;

typedef struct Ast {
    AstKind kind;
    // children / linked list
    struct Ast *next; // next sibling
    struct Ast *child; // first child
    // payload
    char *name; // identifier or function name
    int int_value; // for integer literals
    char *lexeme; // optional lexeme
} Ast;

// helpers
Ast *ast_new(AstKind kind);
void ast_free(Ast *a);
void ast_append_child(Ast *parent, Ast *child);
void ast_print(Ast *a, int indent);
// convenience for printing root
static inline void ast_print_root(Ast *a) { ast_print(a, 0); }

#endif // AST_H
