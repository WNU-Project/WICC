#include <stdio.h>
#include <string.h>
#include "interpiler.h"

static void execute_node(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_FUNCTION:
            // For now, just run all children of main
            for (int i = 0; i < node->child_count; i++) {
                execute_node(node->children[i]);
            }
            break;

        case AST_PRINT:
            printf("%s\n", node->value);
            break;

        case AST_RETURN:
            // For now, just show return value
            printf("Program returned: %s\n", node->value);
            break;

        default:
            printf("Unknown node type\n");
            break;
    }
}

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
