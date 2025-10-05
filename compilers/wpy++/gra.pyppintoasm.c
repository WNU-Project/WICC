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

// Safe NASM string emitter for graphics (same logic as console version)
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

// Emit a string label and replace the node's value with that label.
// Assumes node->value is an unquoted string literal, e.g., "Hello".
static void emit_string_label(ASTNode *lit, FILE *out) {
    char label[32];
    snprintf(label, sizeof(label), "str%d", str_counter++);

    // The string is already unquoted, so use it directly
    const char *str_content = lit->value;
    if (str_content) {
        // Process escape sequences and emit safe NASM string
        emit_nasm_db_string(out, label, str_content);
    } else {
        // Fallback for NULL strings
        fprintf(out, "%s db \"ERROR\", 0\n", label);
    }

    // Replace node->value with the label so codegen can use [rel label]
    free(lit->value);
    lit->value = my_strdup(label);
}

// Walk the AST and emit all string literals into the single .data section (labels only).
static void collect_strings(ASTNode *node, FILE *out) {
    if (!node) return;

    if (node->type == AST_LITERAL && node->value && node->lit_kind == LIT_STRING) {
        emit_string_label(node, out);
    }

    for (int i = 0; i < node->child_count; i++) {
        collect_strings(node->children[i], out);
    }
}

int gra_generate_asm_to_file(ASTNode *ast) {
    printf("DEBUG: gra_generate_asm_to_file called!\n");
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
    // Windows x64 calling convention - reserve shadow space for function calls
    fprintf(out, "    sub rsp, 32\n");
    
    // WinMain parameters are in RCX, RDX, R8, R9 but we don't use them
    // Just proceed with graphics initialization

    // Generate code for main()
    printf("DEBUG: About to process AST with %d children\n", ast->child_count);
    for (int i = 0; i < ast->child_count; i++) {
        printf("DEBUG: Processing child %d\n", i);
        gen_node(ast->children[i], out);
    }
    printf("DEBUG: Finished processing all children\n");

    // Restore stack and return success
    fprintf(out, "    add rsp, 32\n");
    fprintf(out, "    xor eax, eax\n");  // return 0 (success)
    fprintf(out, "    ret\n");

    fclose(out);
    printf("Assembly written to outgra.asm (Win32 GDI)\n");
    return 0;
}

static void gen_node(ASTNode *node, FILE *out) {
    if (!node) {
        printf("DEBUG: gen_node called with NULL node\n");
        return;
    }

    printf("DEBUG: gen_node called with node type: %d (AST_CALL=%d)\n", node->type, AST_CALL);
    if (node->value) {
        printf("DEBUG: node value: '%s'\n", node->value);
    }

    switch (node->type) {
        case AST_FUNCTION:
            printf("DEBUG: In AST_FUNCTION case\n");
            if (node->value && strcmp(node->value, "main") == 0) {
                for (int i = 0; i < node->child_count; i++) {
                    gen_node(node->children[i], out);
                }
            }
            break;

        case AST_BLOCK:
            printf("DEBUG: In AST_BLOCK case\n");
            for (int i = 0; i < node->child_count; i++) {
                gen_node(node->children[i], out);
            }
            break;

        case AST_IDENTIFIER:
            printf("DEBUG: Found AST_IDENTIFIER node with value: %s\n", node->value ? node->value : "NULL");
            if (node->value && strncmp(node->value, "graphics.", 9) == 0) {
                const char *method = node->value + 9; // e.g., "Init"
                printf("DEBUG: Processing graphics.%s call\n", method);

                // Windows x64 calling convention: RCX, RDX, R8, R9
                if (strcmp(method, "Init") == 0) {
                    // graphics.Init(title: string, width: int, height: int)
                    ASTNode *title = node->children[0];  // LITERAL node
                    ASTNode *w     = node->children[1];  // LITERAL node  
                    ASTNode *h     = node->children[2];  // LITERAL node

                    fprintf(out, "    lea rcx, [rel %s]\n", title->value); // title (already a label)
                    fprintf(out, "    mov rdx, %s\n", w->value);           // width
                    fprintf(out, "    mov r8, %s\n", h->value);            // height
                    fprintf(out, "    call graphics_Init\n");
                }
                else if (strcmp(method, "Clear") == 0) {
                    // graphics.Clear(color: string)
                    ASTNode *color = node->children[0];  // LITERAL node
                    
                    fprintf(out, "    lea rcx, [rel %s]\n", color->value); // color (already a label)
                    fprintf(out, "    call graphics_Clear\n");
                }
                else if (strcmp(method, "DrawText") == 0) {
                    // graphics.DrawText(x: int, y: int, text: string)
                    ASTNode *x    = node->children[0];  // LITERAL node
                    ASTNode *y    = node->children[1];  // LITERAL node
                    ASTNode *text = node->children[2];  // LITERAL node

                    fprintf(out, "    mov rcx, %s\n", x->value);          // x
                    fprintf(out, "    mov rdx, %s\n", y->value);          // y
                    fprintf(out, "    lea r8, [rel %s]\n", text->value);  // text (already a label)
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
            printf("DEBUG: Unhandled node type %d with value '%s'\n", node->type, node->value ? node->value : "NULL");
            break;
    }
}
