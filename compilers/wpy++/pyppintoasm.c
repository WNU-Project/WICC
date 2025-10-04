#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

// -------- Codegen context --------
typedef struct {
    int str_index;   // running index for string labels
} CodegenCtx;

// Forward declarations
static void gen_node(ASTNode *node, FILE *out, CodegenCtx *ctx);
static void emit_data_section(ASTNode *ast, FILE *out);
static void emit_text_section_header(FILE *out);

// -------- Safe NASM string emitter --------
// Emits a NASM-safe db line: printable bytes in quotes (with doubled `"`),
// control bytes as numeric literals, and always appends a null terminator.
static void emit_nasm_db_string(FILE *out, const char *label, const char *s) {
    fprintf(out, "%s db ", label);

    int open = 0;
    int first_item = 1;
    
    for (size_t i = 0; s[i]; i++) {
        unsigned char b = (unsigned char)s[i];
        int printable = (b >= 32 && b != 127);

        if (printable && b != '"') {
            if (!open) { 
                if (!first_item) fprintf(out, ", ");
                fprintf(out, "\""); 
                open = 1; 
                first_item = 0;
            }
            fputc(b, out);
        } else if (printable && b == '"') {
            if (!open) { 
                if (!first_item) fprintf(out, ", ");
                fprintf(out, "\""); 
                open = 1; 
                first_item = 0;
            }
            fprintf(out, "\"\""); // NASM doubles quotes inside strings
        } else {
            if (open) { 
                fprintf(out, "\""); 
                open = 0; 
            }
            if (!first_item) fprintf(out, ", ");
            fprintf(out, "%u", b);
            first_item = 0;
        }
    }
    if (open) fprintf(out, "\"");
    fprintf(out, ", 0\n"); // null terminator
}

// -------- Public API: always write to "out.asm" --------
int generate_asm_to_file(ASTNode *ast) {
    const char *out_path = "out.asm";
    FILE *out = fopen(out_path, "w");
    if (!out) {
        perror("generate_asm_to_file: fopen");
        return 1;
    }

    // 1) Data section
    emit_data_section(ast, out);

    // 2) Text section
    emit_text_section_header(out);

    // 3) Walk AST
    CodegenCtx ctx = {0};
    for (int i = 0; i < ast->child_count; i++) {
        gen_node(ast->children[i], out, &ctx);
    }

    fclose(out);
    printf("Assembly written to %s\n", out_path);
    return 0;
}

// -------- Helpers --------
static void emit_data_section(ASTNode *ast, FILE *out) {
    fprintf(out, "section .data\n");

    // Safe printf format string
    fprintf(out, "fmt_str db \"%%s\", 0\n");

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
                        char label[32];
                        snprintf(label, sizeof(label), "str%d", strCount);
                        emit_nasm_db_string(out, label, lit->value);
                        strCount++;
                    }
                }
            }
        }
    }
    fprintf(out, "\n");
}

static void emit_text_section_header(FILE *out) {
    fprintf(out, "section .text\n");
    fprintf(out, "extern printf\n");
    fprintf(out, "extern exit\n");
    fprintf(out, "global main\n\n");
}

// -------- Code emission --------
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
            // Windows x64 ABI: RCX = format, RDX = string
            fprintf(out, "    sub rsp, 32              ; shadow space\n");
            fprintf(out, "    lea rcx, [rel fmt_str]   ; RCX = \"%%s\"\n");
            fprintf(out, "    lea rdx, [rel str%d]     ; RDX = &string\n", ctx->str_index);
            fprintf(out, "    call printf\n");
            fprintf(out, "    add rsp, 32\n");
            ctx->str_index++;
            break;

        case AST_RETURN: {
            const char *imm = "0";
            if (node->child_count > 0 && node->children[0] && node->children[0]->value) {
                imm = node->children[0]->value;
            }
            fprintf(out, "    sub rsp, 32              ; shadow space\n");
            fprintf(out, "    mov ecx, %s              ; RCX = exit code\n", imm);
            fprintf(out, "    call exit\n");
            fprintf(out, "    add rsp, 32\n");
            break;
        }

        default:
            break;
    }
}
