#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

typedef struct {
    int str_index;
} CodegenCtx;

static void gen_node(ASTNode *node, FILE *out, CodegenCtx *ctx);
static void emit_data_section(ASTNode *ast, FILE *out);
static void emit_text_header(FILE *out);

int generate_asm32(ASTNode *ast) {
    const char *out_path = "out32.asm";
    FILE *out = fopen(out_path, "w");
    if (!out) {
        perror("fopen out32.asm");
        return 1;
    }

    emit_data_section(ast, out);
    emit_text_header(out);

    CodegenCtx ctx = {0};
    for (int i = 0; i < ast->child_count; i++) {
        gen_node(ast->children[i], out, &ctx);
    }

    fclose(out);
    printf("Assembly written to %s\n", out_path);
    return 0;
}

static void emit_data_section(ASTNode *ast, FILE *out) {
    fprintf(out, "section .data\n");

    int strCount = 0;
    for (int i = 0; i < ast->child_count; i++) {
        ASTNode *func = ast->children[i];
        if (func && func->type == AST_FUNCTION && func->child_count > 0) {
            ASTNode *body = func->children[0];
            for (int j = 0; j < body->child_count; j++) {
                ASTNode *stmt = body->children[j];
                if (stmt && stmt->type == AST_PRINT && stmt->child_count > 0) {
                    ASTNode *lit = stmt->children[0];
                    if (lit && lit->type == AST_LITERAL && lit->value) {
                        fprintf(out, "str%d db %s, 0\n", strCount, lit->value);
                        strCount++;
                    }
                }
            }
        }
    }
    fprintf(out, "\n");
}

static void emit_text_header(FILE *out) {
    fprintf(out, "section .text\n");
    fprintf(out, "extern printf\n");
    fprintf(out, "extern exit\n");
    fprintf(out, "global main\n\n");
}

static void gen_node(ASTNode *node, FILE *out, CodegenCtx *ctx) {
    if (!node) return;

    switch (node->type) {
        case AST_FUNCTION:
            if (node->value && strcmp(node->value, "main") == 0) {
                fprintf(out, "main:\n");
                for (int i = 0; i < node->child_count; i++) {
                    gen_node(node->children[i], out, ctx);
                }
            }
            break;

        case AST_BLOCK:
            for (int i = 0; i < node->child_count; i++) {
                gen_node(node->children[i], out, ctx);
            }
            break;

        case AST_PRINT:
            fprintf(out, "    push str%d\n", ctx->str_index);
            fprintf(out, "    call printf\n");
            fprintf(out, "    add esp, 4\n");
            ctx->str_index++;
            break;

        case AST_RETURN: {
            const char *imm = "0";
            if (node->child_count > 0 && node->children[0] && node->children[0]->value) {
                imm = node->children[0]->value;
            }
            fprintf(out, "    push %s\n", imm);
            fprintf(out, "    call exit\n");
            break;
        }

        default:
            break;
    }
}
