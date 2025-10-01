#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

// AST node types
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_BLOCK,
    AST_PRINT,
    AST_RETURN,
    AST_LITERAL,
    AST_IDENTIFIER,
    AST_CALL,
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    char *value;                 // e.g. identifier name or literal text
    struct ASTNode **children;   // child nodes
    int child_count;
} ASTNode;

// Parser API
ASTNode *parse(Token *tokens, int count);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int indent);

#endif
