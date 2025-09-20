#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

typedef enum {
    AST_FUNCTION,
    AST_PRINT,
    AST_RETURN,
    AST_LITERAL,
    AST_IDENTIFIER
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    char *value;                // e.g. function name, string literal
    struct ASTNode **children;  // array of child nodes
    int child_count;
} ASTNode;

ASTNode *parse(Token *tokens, int token_count);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int indent);

#endif
