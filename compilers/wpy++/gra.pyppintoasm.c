// gra_pyppintoasm.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "gra_pyppintoasm.h"

static void gen_node(ASTNode *node, FILE *out);
static void collect_strings(ASTNode *node, FILE *out);

// Portable strdup replacement (avoids _strdup warnings on MinGW)
static char *my_strdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = (char*)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

static int str_counter = 0;

// Emit a string label and replace the node's value with that label.
// Assumes node->value is a quoted string literal, e.g., "Hello".
static void emit_string_label(ASTNode *lit, FILE *out) {
    char label[32];
    snprintf(label, sizeof(label), "str%d", str_counter++);

    // Write the literal into .data as-is (NASM accepts "..." with ,0 for null-termination)
    fprintf(out, "%s db %s,0\n", label, lit->value);

    // Replace node->value with the label so codegen can use [rel label]
    free(lit->value);
    lit->value = my_strdup(label);
}

// Walk the AST and emit all string literals into the single .data section (labels only).
static void collect_strings(ASTNode *node, FILE *out) {
    if (!node) return;

    if (node->type == AST_LITERAL && node->value && node->value[0] == '"') {
        emit_string_label(node, out);
    }

    for (int i = 0; i < node->child_count; i++) {
        collect_strings(node->children[i], out);
    }
}

int gra_generate_asm_to_file(ASTNode *ast) {
    FILE *out = fopen("outgra.asm", "w");
    if (!out) { perror("gra_generate_asm_to_file: fopen"); return 1; }

    // Single data section: emit all string literals as labels
    fprintf(out, "section .data\n");
    collect_strings(ast, out);
    fprintf(out, "\n");

    // Code section and externs
    fprintf(out, "section .text\n");
    fprintf(out, "extern graphics_Init\n");
    fprintf(out, "extern graphics_Clear\n");
    fprintf(out, "extern graphics_DrawText\n");
    fprintf(out, "extern graphics_DrawRect\n");
    fprintf(out, "extern graphics_Loop\n");
    fprintf(out, "global WinMain\n\n");

    fprintf(out, "WinMain:\n");

    // Generate code for main()
    for (int i = 0; i < ast->child_count; i++) {
        gen_node(ast->children[i], out);
    }

    fprintf(out, "    xor eax, eax\n");
    fprintf(out, "    ret\n");

    fclose(out);
    printf("Assembly written to outgra.asm (Win32 GDI)\n");
    return 0;
}

static void gen_node(ASTNode *node, FILE *out) {
    if (!node) return;

    switch (node->type) {
        case AST_FUNCTION:
            if (node->value && strcmp(node->value, "main") == 0) {
                for (int i = 0; i < node->child_count; i++) {
                    gen_node(node->children[i], out);
                }
            }
            break;

        case AST_BLOCK:
            for (int i = 0; i < node->child_count; i++) {
                gen_node(node->children[i], out);
            }
            break;

        case AST_IDENTIFIER:
            if (node->value && strncmp(node->value, "graphics.", 9) == 0) {
                const char *method = node->value + 9; // e.g., "Init"

                // Windows x64 calling convention: RCX, RDX, R8, R9
                if (strcmp(method, "Init") == 0) {
                    // graphics.Init(title: string, width: int, height: int)
                    ASTNode *title = node->children[0];
                    ASTNode *w     = node->children[1];
                    ASTNode *h     = node->children[2];

                    fprintf(out, "    lea rcx, [rel %s]\n", title->value); // title
                    fprintf(out, "    mov rdx, %s\n", w->value);           // width
                    fprintf(out, "    mov r8, %s\n", h->value);            // height
                    fprintf(out, "    call graphics_Init\n");
                }
                else if (strcmp(method, "Clear") == 0) {
                    // graphics.Clear(color: string)
                    ASTNode *color = node->children[0];
                    fprintf(out, "    lea rcx, [rel %s]\n", color->value);
                    fprintf(out, "    call graphics_Clear\n");
                }
                else if (strcmp(method, "DrawText") == 0) {
                    // graphics.DrawText(x: int, y: int, text: string)
                    ASTNode *x    = node->children[0];
                    ASTNode *y    = node->children[1];
                    ASTNode *text = node->children[2];

                    fprintf(out, "    mov rcx, %s\n", x->value);          // x
                    fprintf(out, "    mov rdx, %s\n", y->value);          // y
                    fprintf(out, "    lea r8, [rel %s]\n", text->value);  // text
                    fprintf(out, "    call graphics_DrawText\n");
                }
                else if (strcmp(method, "DrawRect") == 0) {
                    // graphics.DrawRect(x: int, y: int, w: int, h: int)
                    ASTNode *x = node->children[0];
                    ASTNode *y = node->children[1];
                    ASTNode *w = node->children[2];
                    ASTNode *h = node->children[3];

                    fprintf(out, "    mov rcx, %s\n", x->value); // x
                    fprintf(out, "    mov rdx, %s\n", y->value); // y
                    fprintf(out, "    mov r8, %s\n", w->value);  // w
                    fprintf(out, "    mov r9, %s\n", h->value);  // h
                    fprintf(out, "    call graphics_DrawRect\n");
                }
                else if (strcmp(method, "Loop") == 0) {
                    fprintf(out, "    call graphics_Loop\n");
                }
            }
            break;

        default:
            break;
    }
}
