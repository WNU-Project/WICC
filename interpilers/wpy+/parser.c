#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Track includes
static int has_pypstdio = 0;

// Utility to create AST nodes
static ASTNode *make_node(ASTNodeType type, const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

// Minimal parser: supports #include <pypstdio> and func main() { ... }
ASTNode *parse(Token *tokens, int token_count) {
    if (token_count < 1) return NULL;

    int i = 0;

    // --- Handle includes ---
    while (i < token_count && tokens[i].type == TOKEN_INCLUDE) {
        // Expect < identifier >
        if (i+3 < token_count &&
            tokens[i+1].type == TOKEN_LT &&
            tokens[i+2].type == TOKEN_IDENTIFIER &&
            tokens[i+3].type == TOKEN_GT) {

            if (strcmp(tokens[i+2].lexeme, "pypstdio") == 0) {
                has_pypstdio = 1;
            }

            i += 4; // skip past include <name>
        } else {
            fprintf(stderr, "Parse error: malformed #include directive\n");
            return NULL;
        }
    }

    // --- Expect func ---
    if (i >= token_count || tokens[i].type != TOKEN_FUNC) {
        fprintf(stderr, "Parse error: expected 'func'\n");
        return NULL;
    }

    // Expect function name
    if (i+1 >= token_count || tokens[i+1].type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parse error: expected function name\n");
        return NULL;
    }

    ASTNode *func = make_node(AST_FUNCTION, tokens[i+1].lexeme);

    // --- Scan body for print/return ---
    for (int j = i+2; j < token_count; j++) {
        // Detect pypstdio.print
        if (tokens[j].type == TOKEN_IDENTIFIER &&
            strcmp(tokens[j].lexeme, "pypstdio") == 0) {

            if (!has_pypstdio) {
                fprintf(stderr, "Semantic error: 'pypstdio' used without #include <pypstdio>\n");
                free_ast(func);
                return NULL;
            }

            if (j+2 < token_count &&
                tokens[j+1].type == TOKEN_DOT &&
                strcmp(tokens[j+2].lexeme, "print") == 0) {

                if (j+4 < token_count &&
                    tokens[j+3].type == TOKEN_LPAREN &&
                    tokens[j+4].type == TOKEN_STRING) {

                    ASTNode *print = make_node(AST_PRINT, tokens[j+4].lexeme);
                    func->children = realloc(func->children, sizeof(ASTNode*) * (func->child_count + 1));
                    func->children[func->child_count++] = print;
                }
            }
        }

        // Handle bare print("...") only if not part of pypstdio.print
        if (tokens[j].type == TOKEN_IDENTIFIER &&
            strcmp(tokens[j].lexeme, "print") == 0) {
            if ((j == 0 || tokens[j-1].type != TOKEN_DOT) &&
                j+2 < token_count &&
                tokens[j+1].type == TOKEN_LPAREN &&
                tokens[j+2].type == TOKEN_STRING) {

                ASTNode *print = make_node(AST_PRINT, tokens[j+2].lexeme);
                func->children = realloc(func->children, sizeof(ASTNode*) * (func->child_count + 1));
                func->children[func->child_count++] = print;
            }
        }

        // Handle return
        if (tokens[j].type == TOKEN_RETURN && j+1 < token_count) {
            ASTNode *ret = make_node(AST_RETURN, tokens[j+1].lexeme);
            func->children = realloc(func->children, sizeof(ASTNode*) * (func->child_count + 1));
            func->children[func->child_count++] = ret;
        }
    }

    return func;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }
    free(node->children);
    free(node->value);
    free(node);
}

void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    switch (node->type) {
        case AST_FUNCTION: printf("Function: %s\n", node->value); break;
        case AST_PRINT:    printf("Print: %s\n", node->value); break;
        case AST_RETURN:   printf("Return: %s\n", node->value); break;
        default:           printf("Node\n"); break;
    }
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], indent + 1);
    }
}
