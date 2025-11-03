/*
* WNU C Compiler (wnu-c)
* Main entry point
* C11 Language Standard (more standards coming soon)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <llvm-c/Core.h>
#include "tokens.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printf("wnu-c: WNU C Compiler\n");
            printf("Usage: wnu-c <source_file.c> [options]\n");
            printf("Options:\n");
            printf("  --help    Show this help message\n");
            printf("  --version Show version information\n");
            return 0;
        }
    
            if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            printf("WNU C Compiler (wnucc) - C11 Standard\n");
            printf("Copyright (C) 2025 WNU Project\n");
            printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n");
            printf("This is free software: you are free to change and redistribute it.\n");
            printf("There is NO WARRANTY, to the extent permitted by law.\n");
                return 0;
        }
    }

    if (argc < 2) {
        printf("wnu-c: \033[1;31mfatal error:\033[0m no input files\n");
        printf("Usage: wnu-c <source_file.c> [options]\n");
        return 1;
    }

    const char *input_file = argv[1];
    printf("Compiling %s...\n", input_file);

    // Initialize lexer with input file
    if (lexer_init(input_file) != 0) {
        fprintf(stderr, "wnu-c: \033[1;31mfatal error:\033[0m cannot open input file '%s'\n", input_file);
        return 1;
    }

    // Parse translation unit into AST
    if (parser_init(input_file) != 0) {
        fprintf(stderr, "wnu-c: \033[1;31mfatal error:\033[0m parser initialization failed\n");
        return 1;
    }

    Ast *tu = parser_parse_translation_unit();
    if (!tu) {
        fprintf(stderr, "wnu-c: \033[1;31mfatal error:\033[0m parsing failed\n");
        parser_free();
        return 1;
    }

    printf("--- AST ---\n");
    ast_print_root(tu);

    // Generate LLVM IR from AST
    if (codegen_init("wnu_module") != 0) {
        fprintf(stderr, "wnu-c: error: codegen initialization failed\n");
    } else {
        codegen_codegen_ast(tu);
        // write IR to file for inspection
        codegen_write_ir("out.ll");
        codegen_print_ir();
        codegen_dispose();
    }

    ast_free(tu);
    parser_free();

    printf("Compilation finished successfully. LLVM IR written to out.ll\n");
    return 0;
}