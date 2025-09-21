#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpiler.h"

// -----------------------------
// Simple symbol table
// -----------------------------
typedef struct {
    char *name;
    int   int_value;
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
            // Run all children of the function
            for (int i = 0; i < node->child_count; i++) {
                execute_node(node->children[i]);
            }
            break;

        case AST_VAR_DECL:
            if (strcmp(node->var_type, "int") == 0) {
                variables[var_count].name = strdup(node->var_name);
                variables[var_count].int_value = atoi(node->var_value);
                var_count++;
            }
            break;

        case AST_PRINT:
            for (int i = 0; i < node->child_count; i++) {
                ASTNode *arg = node->children[i];
                if (arg->type == AST_LITERAL) {
                    printf("%s", arg->value);
                } else if (arg->type == AST_IDENTIFIER) {
                    // Look up variable
                    int found = 0;
                    for (int v = 0; v < var_count; v++) {
                        if (strcmp(variables[v].name, arg->value) == 0) {
                            printf("%d", variables[v].int_value);
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
            // Print identifier value if it exists
            for (int v = 0; v < var_count; v++) {
                if (strcmp(variables[v].name, node->value) == 0) {
                    printf("%d", variables[v].int_value);
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
