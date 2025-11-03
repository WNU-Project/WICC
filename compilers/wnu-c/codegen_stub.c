#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>

int codegen_init(const char *module_name) {
    (void)module_name;
    /* No-op stub when LLVM is not available */
    return 0;
}

int codegen_codegen_ast(Ast *tu) {
    (void)tu;
    /* Nothing to generate; success */
    return 0;
}

void codegen_print_ir(void) {
    printf("; stub IR - LLVM not available.\n");
}

int codegen_write_ir(const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fprintf(f, "; stub IR - LLVM not available.\n");
    fclose(f);
    return 0;
}

void codegen_dispose(void) {
    /* nothing */
}
