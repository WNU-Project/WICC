#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static int has_pypstdio = 0;

// -----------------------------
// Safe strdup replacement
// -----------------------------
static char *strdup_local(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char *result = (char *)malloc(n + 1);
    if (!result) return NULL;
    memcpy(result, s, n);
    result[n] = '\0';
    return result;
}

// -----------------------------
// AST node constructors
// -----------------------------
static ASTNode *make_node(ASTNodeType type, const char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    node->value = value ? strdup_local(value) : NULL;
    node->var_type = NULL;
    node->var_name = NULL;
    node->var_value = NULL;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

static ASTNode *make_var_decl(const char *type, const char *name, const char *value) {
    ASTNode *node = make_node(AST_VAR_DECL, NULL);
    node->var_type  = strdup_local(type);
    node->var_name  = strdup_local(name);
    node->var_value = strdup_local(value);
    return node;
}

// -----------------------------
// Parser
// -----------------------------
ASTNode *parse(Token *tokens, int token_count) {
    if (token_count < 1) return NULL;
    int i = 0;

    // Handle includes
    while (i < token_count && tokens[i].type == TOKEN_INCLUDE) {
        if (tokens[i].lexeme && strstr(tokens[i].lexeme, "pypstdio")) {
            has_pypstdio = 1;
        }
        i++;
    }

    // Expect func
    if (i >= token_count || tokens[i].type != TOKEN_FUNC) {
        fprintf(stderr, "Parse error: expected 'func'\n");
        return NULL;
    }

    // Expect function name
    if (i + 1 >= token_count || tokens[i + 1].type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parse error: expected function name\n");
        return NULL;
    }

    ASTNode *func = make_node(AST_FUNCTION, tokens[i + 1].lexeme);

    // Scan body
    for (int j = i + 2; j < token_count; j++) {
        if (tokens[j].type == TOKEN_IDENTIFIER &&
            strcmp(tokens[j].lexeme, "pypstdio") == 0) {

            if (!has_pypstdio) {
                fprintf(stderr, "Semantic error: 'pypstdio' used without #include <pypstdio>\n");
                free_ast(func);
                return NULL;
            }

            // Variable declarations
            if (j + 4 < token_count &&
                tokens[j + 1].type == TOKEN_DOT &&
                strcmp(tokens[j + 2].lexeme, "variable") == 0 &&
                tokens[j + 3].type == TOKEN_DOT) {

                const char *type = tokens[j + 4].lexeme;

                // int and char (plain)
                if ((strcmp(type, "int") == 0 || strcmp(type, "char") == 0) && j + 5 < token_count) {
                    int k = j + 5;
                    // If next is a dot, this is likely char.str; let the string branch handle it.
                    if (tokens[k].type == TOKEN_DOT) {
                        // fall through to char.str handler below
                    } else {
                        if (tokens[k].type != TOKEN_LPAREN) continue;
                        k++;
                        if (k >= token_count || tokens[k].type != TOKEN_IDENTIFIER) continue;
                        char *var_name = tokens[k].lexeme;
                        k++;
                        if (k >= token_count || tokens[k].type != TOKEN_COMMA) continue;
                        k++;

                        char *var_value = NULL;
                        if (strcmp(type, "int") == 0 && tokens[k].type == TOKEN_NUMBER) {
                            var_value = tokens[k].lexeme;
                        } else if (strcmp(type, "char") == 0 && tokens[k].type == TOKEN_CHAR_LITERAL) {
                            var_value = tokens[k].lexeme;
                        } else {
                            continue;
                        }

                        k++;
                        if (k >= token_count || tokens[k].type != TOKEN_RPAREN) continue;
                        k++;
                        if (k >= token_count || tokens[k].type != TOKEN_SEMICOLON) continue;
                        k++;

                        ASTNode *decl = make_var_decl(type, var_name, var_value);
                        func->children = (ASTNode **)realloc(func->children, sizeof(ASTNode *) * (func->child_count + 1));
                        func->children[func->child_count++] = decl;
                        j = k - 1;
                        continue;
                    }
                }

                // string: pypstdio.variable.char.str(name, "value");
                if (strcmp(type, "char") == 0 &&
                    j + 6 < token_count &&
                    tokens[j + 5].type == TOKEN_DOT &&
                    strcmp(tokens[j + 6].lexeme, "str") == 0) {

                    int k = j + 7;
                    if (tokens[k].type != TOKEN_LPAREN) continue;
                    k++;
                    if (k >= token_count || tokens[k].type != TOKEN_IDENTIFIER) continue;
                    char *var_name = tokens[k].lexeme;
                    k++;
                    if (k >= token_count || tokens[k].type != TOKEN_COMMA) continue;
                    k++;
                    if (k >= token_count || tokens[k].type != TOKEN_STRING) continue;
                    char *var_value = tokens[k].lexeme;
                    k++;
                    if (k >= token_count || tokens[k].type != TOKEN_RPAREN) continue;
                    k++;
                    if (k >= token_count || tokens[k].type != TOKEN_SEMICOLON) continue;
                    k++;

                    ASTNode *decl = make_var_decl("string", var_name, var_value);
                    func->children = (ASTNode **)realloc(func->children, sizeof(ASTNode *) * (func->child_count + 1));
                    func->children[func->child_count++] = decl;
                    j = k - 1;
                    continue;
                }
            }

            // Print statement
            if (j + 3 < token_count &&
                tokens[j + 1].type == TOKEN_DOT &&
                strcmp(tokens[j + 2].lexeme, "print") == 0 &&
                tokens[j + 3].type == TOKEN_LPAREN) {

                int k = j + 4;
                ASTNode *print = make_node(AST_PRINT, NULL);

                while (k < token_count && tokens[k].type != TOKEN_RPAREN) {
                    if (tokens[k].type == TOKEN_STRING || tokens[k].type == TOKEN_CHAR_LITERAL) {
                        ASTNode *lit = make_node(AST_LITERAL, tokens[k].lexeme);
                        print->children = (ASTNode **)realloc(print->children, sizeof(ASTNode *) * (print->child_count + 1));
                        print->children[print->child_count++] = lit;
                    } else if (tokens[k].type == TOKEN_IDENTIFIER) {
                        ASTNode *id = make_node(AST_IDENTIFIER, tokens[k].lexeme);
                        print->children = (ASTNode **)realloc(print->children, sizeof(ASTNode *) * (print->child_count + 1));
                        print->children[print->child_count++] = id;
                    }
                    k++;
                    if (k < token_count && tokens[k].type == TOKEN_COMMA) k++;
                }

                if (k >= token_count || tokens[k].type != TOKEN_RPAREN) continue;
                k++;
                if (k >= token_count || tokens[k].type != TOKEN_SEMICOLON) continue;
                k++;

                func->children = (ASTNode **)realloc(func->children, sizeof(ASTNode *) * (func->child_count + 1));
                func->children[func->child_count++] = print;
                j = k - 1;
                continue;
            }
        }

        // Return statement
        if (tokens[j].type == TOKEN_RETURN && j + 1 < token_count) {
            ASTNode *ret = make_node(AST_RETURN, tokens[j + 1].lexeme);
            func->children = (ASTNode **)realloc(func->children, sizeof(ASTNode *) * (func->child_count + 1));
            func->children[func->child_count++] = ret;
        }
    }

    return func;
}

// -----------------------------
// AST utilities
// -----------------------------
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
        case AST_FUNCTION:
            printf("Function: %s\n", node->value);
            break;
        case AST_PRINT:
            printf("Print\n");
            break;
        case AST_LITERAL:
            printf("Literal: %s\n", node->value);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->value);
            break;
        case AST_RETURN:
            printf("Return: %s\n", node->value);
            break;
        case AST_VAR_DECL:
            printf("VarDecl: type=%s name=%s value=%s\n",
                   node->var_type ? node->var_type : "(null)",
                   node->var_name ? node->var_name : "(null)",
                   node->var_value ? node->var_value : "(null)");
            break;
        default:
            printf("Node\n");
            break;
    }
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], indent + 1);
    }
}
