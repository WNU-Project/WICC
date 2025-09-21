#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#define _POSIX_C_SOURCE 200809L

// Track includes
static int has_pypstdio = 0;

// Utility to create AST nodes
static ASTNode *make_node(ASTNodeType type, const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->var_type = NULL;
    node->var_name = NULL;
    node->var_value = NULL;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

// Specialized constructor for variable declarations
static ASTNode *make_var_decl(const char *type, const char *name, const char *value) {
    ASTNode *node = make_node(AST_VAR_DECL, NULL);
    node->var_type = strdup(type);
    node->var_name = strdup(name);
    node->var_value = strdup(value);
    return node;
}

// Minimal parser: supports #include <pypstdio>, func main() { ... }, print, return, and variable decls
ASTNode *parse(Token *tokens, int token_count) {
    if (token_count < 1) return NULL;

    int i = 0;

    // --- Handle includes ---
    while (i < token_count && tokens[i].type == TOKEN_INCLUDE) {
        if (tokens[i].lexeme && strstr(tokens[i].lexeme, "pypstdio")) {
            has_pypstdio = 1;
        }
        i++;
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

    // --- Scan body for statements ---
    for (int j = i+2; j < token_count; j++) {
        // Detect pypstdio.variable.int(...)
        if (tokens[j].type == TOKEN_IDENTIFIER &&
            strcmp(tokens[j].lexeme, "pypstdio") == 0) {

            if (!has_pypstdio) {
                fprintf(stderr, "Semantic error: 'pypstdio' used without #include <pypstdio>\n");
                free_ast(func);
                return NULL;
            }

            // Variable declaration
            if (j+4 < token_count &&
                tokens[j+1].type == TOKEN_DOT &&
                strcmp(tokens[j+2].lexeme, "variable") == 0 &&
                tokens[j+3].type == TOKEN_DOT &&
                strcmp(tokens[j+4].lexeme, "int") == 0) {

                int k = j+5;
                if (tokens[k].type != TOKEN_LPAREN) { fprintf(stderr, "Parse error: expected '('\n"); return NULL; }
                k++;

                if (tokens[k].type != TOKEN_IDENTIFIER) { fprintf(stderr, "Parse error: expected variable name\n"); return NULL; }
                char *var_name = tokens[k].lexeme;
                k++;

                if (tokens[k].type != TOKEN_COMMA) { fprintf(stderr, "Parse error: expected ','\n"); return NULL; }
                k++;

                if (tokens[k].type != TOKEN_NUMBER) { fprintf(stderr, "Parse error: expected integer literal\n"); return NULL; }
                char *var_value = tokens[k].lexeme;
                k++;

                if (tokens[k].type != TOKEN_RPAREN) { fprintf(stderr, "Parse error: expected ')'\n"); return NULL; }
                k++;

                if (tokens[k].type != TOKEN_SEMICOLON) { fprintf(stderr, "Parse error: expected ';'\n"); return NULL; }
                k++;

                ASTNode *decl = make_var_decl("int", var_name, var_value);
                func->children = realloc(func->children, sizeof(ASTNode*) * (func->child_count + 1));
                func->children[func->child_count++] = decl;

                j = k-1;
                continue;
            }

            // pypstdio.print(...)
            if (j+2 < token_count &&
                tokens[j+1].type == TOKEN_DOT &&
                strcmp(tokens[j+2].lexeme, "print") == 0 &&
                j+3 < token_count &&
                tokens[j+3].type == TOKEN_LPAREN) {

                int k = j+4;
                ASTNode *print = make_node(AST_PRINT, NULL);

                // Parse arguments until ')'
                while (k < token_count && tokens[k].type != TOKEN_RPAREN) {
                    if (tokens[k].type == TOKEN_STRING) {
                        ASTNode *lit = make_node(AST_LITERAL, tokens[k].lexeme);
                        print->children = realloc(print->children, sizeof(ASTNode*) * (print->child_count + 1));
                        print->children[print->child_count++] = lit;
                    } else if (tokens[k].type == TOKEN_IDENTIFIER) {
                        ASTNode *id = make_node(AST_IDENTIFIER, tokens[k].lexeme);
                        print->children = realloc(print->children, sizeof(ASTNode*) * (print->child_count + 1));
                        print->children[print->child_count++] = id;
                    }
                    k++;
                    if (tokens[k].type == TOKEN_COMMA) k++;
                }

                if (tokens[k].type != TOKEN_RPAREN) { fprintf(stderr, "Parse error: expected ')'\n"); return NULL; }
                k++;

                if (tokens[k].type != TOKEN_SEMICOLON) { fprintf(stderr, "Parse error: expected ';'\n"); return NULL; }
                k++;

                func->children = realloc(func->children, sizeof(ASTNode*) * (func->child_count + 1));
                func->children[func->child_count++] = print;

                j = k-1;
                continue;
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
    free(node->var_type);
    free(node->var_name);
    free(node->var_value);
    free(node);
}

void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    switch (node->type) {
        case AST_FUNCTION: printf("Function: %s\n", node->value); break;
        case AST_PRINT:    printf("Print\n"); break;
        case AST_LITERAL:  printf("Literal: %s\n", node->value); break;
        case AST_IDENTIFIER: printf("Identifier: %s\n", node->value); break;
        case AST_RETURN:   printf("Return: %s\n", node->value); break;
        case AST_VAR_DECL: printf("VarDecl: type=%s name=%s value=%s\n",
                                   node->var_type, node->var_name, node->var_value); break;
        default:           printf("Node\n"); break;
    }
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], indent + 1);
    }
}
