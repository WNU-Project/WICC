#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpiler.h"

// -----------------------------
// Safe strdup replacement
// -----------------------------
static char *strdup_local(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char *result = malloc(n + 1);
    if (!result) return NULL;
    memcpy(result, s, n);
    result[n] = '\0';
    return result;
}

// -----------------------------
// Symbol table with type support
// -----------------------------
typedef enum { VAR_INT, VAR_CHAR, VAR_STRING } VarType;

typedef struct {
    char *name;
    VarType type;
    union {
        int int_value;
        char char_value;
        char *string_value;
    };
} Variable;

static Variable variables[256];
static int var_count = 0;

// -----------------------------
// Execution
// -----------------------------
static void execute_node(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_FUNCTION:
            for (int i = 0; i < node->child_count; i++) {
                execute_node(node->children[i]);
            }
            break;

        case AST_VAR_DECL:
            if (strcmp(node->var_type, "int") == 0) {
                variables[var_count].name = strdup_local(node->var_name);
                variables[var_count].type = VAR_INT;
                variables[var_count].int_value = atoi(node->var_value);
                var_count++;
            } else if (strcmp(node->var_type, "char") == 0) {
                variables[var_count].name = strdup_local(node->var_name);
                variables[var_count].type = VAR_CHAR;
                variables[var_count].char_value = node->var_value[0]; // first character
                var_count++;
            } else if (strcmp(node->var_type, "string") == 0) {
             variables[var_count].name = strdup_local(node->var_name);
             variables[var_count].type = VAR_STRING;
             variables[var_count].string_value = strdup_local(node->var_value);
             var_count++;
            }

            break;

        case AST_PRINT:
            for (int i = 0; i < node->child_count; i++) {
                ASTNode *arg = node->children[i];
                if (arg->type == AST_LITERAL) {
                    printf("%s", arg->value);
                } else if (arg->type == AST_IDENTIFIER) {
                    int found = 0;
                    for (int v = 0; v < var_count; v++) {
                        if (strcmp(variables[v].name, arg->value) == 0) {
                            if (variables[v].type == VAR_INT) {
                                printf("%d", variables[v].int_value);
                            } else if (variables[v].type == VAR_CHAR) {
                                printf("%c", variables[v].char_value);
                            } else if (variables[v].type == VAR_STRING) {
                                printf("%s", variables[v].string_value);
                            }
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("[undefined:%s]", arg->value);
                    }
                }
                if (i < node->child_count - 1) printf(" ");
            }
            printf("\n");
            break;

        case AST_RETURN:
            printf("Program returned: %s\n", node->value);
            break;

        case AST_LITERAL:
            printf("%s", node->value);
            break;

        case AST_IDENTIFIER:
            for (int v = 0; v < var_count; v++) {
                if (strcmp(variables[v].name, node->value) == 0) {
                    if (variables[v].type == VAR_INT) {
                        printf("%d", variables[v].int_value);
                    } else if (variables[v].type == VAR_CHAR) {
                        printf("%c", variables[v].char_value);
                    } else if (variables[v].type == VAR_STRING) {
                        printf("%s", variables[v].string_value);
                    }
                    return;
                }
            }
            printf("[undefined:%s]", node->value);
            break;

        default:
            fprintf(stderr, "Unknown node type %d\n", node->type);
            break;
    }
}

// -----------------------------
// Entry points
// -----------------------------
void run_program(ASTNode *root) {
    if (!root) {
        fprintf(stderr, "No AST to run.\n");
        return;
    }

    if (root->type == AST_FUNCTION) {
        printf("Running function: %s\n", root->value);
        execute_node(root);
    } else {
        fprintf(stderr, "Top-level AST is not a function.\n");
    }
}

void interpret(ASTNode *root) {
    if (!root) {
        printf("Nothing to interpret.\n");
        return;
    }
    run_program(root);
}
