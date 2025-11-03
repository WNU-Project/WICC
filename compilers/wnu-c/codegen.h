#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

// Initialize codegen (create module and builder)
int codegen_init(const char *module_name);

// Generate LLVM IR for the AST translation unit. Returns 0 on success.
int codegen_codegen_ast(Ast *tu);

// Print module IR to stdout
void codegen_print_ir(void);

// Write IR to a file path
int codegen_write_ir(const char *path);

// Dispose codegen resources
void codegen_dispose(void);

#endif // CODEGEN_H
