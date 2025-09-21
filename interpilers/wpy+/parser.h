#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

// -----------------------------
// AST Node Types
// -----------------------------
typedef enum {
    AST_FUNCTION,     // func main() { ... }
    AST_PRINT,        // pypstdio.print(...)
    AST_RETURN,       // return ...
    AST_LITERAL,      // string/number literal
    AST_IDENTIFIER,   // variable/function names
    AST_VAR_DECL      // pypstdio.variable.int(name, value)
} ASTNodeType;

// -----------------------------
// AST Node Structures
// -----------------------------
typedef struct ASTNode {
    ASTNodeType type;

    // General value (function name, literal text, identifier name, etc.)
    char *value;

    // For variable declarations
    char *var_type;   // e.g. "int", "string"
    char *var_name;   // e.g. "int1"
    char *var_value;  // e.g. "10"

    // Children (for function bodies, print args, etc.)
    struct ASTNode **children;
    int child_count;
} ASTNode;

// -----------------------------
// Parser API
// -----------------------------
ASTNode *parse(Token *tokens, int token_count);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int indent);

#endif // PARSER_H
