#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokens.h"
#include "parser.h"
#include "interpiler.h"
#include "lexer.h"

int main(int argc, char *argv[]) {
    // Check for arguments
    if (argc < 2) {
        printf("wpy+.exe: \033[1;31mfatal error:\033[0m no arguments provided \033[1;31mError Code: 1\033[0m\n");
        printf("Usage: wpy+.exe <source_file.pyp> [options]\n");
        return 1;
    }

    // Handle flags first
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "help") == 0   || strcmp(argv[1], "h") == 0) {
        printf("wpy+.exe: Python+ Interpreter\n");
        printf("Usage: wpy+.exe <source_file.pyp> [options]\n");
        printf("Options:\n");
        printf("  --help, -h    Show this help message\n");
        printf("  --version, -v Show version information\n");
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        printf("WICC wpy+.exe 0.1.0 (Python+ Interpiler)\n");
        printf("Copyright (C) 2025 WNU Project\n");
        printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n");
        printf("This is free software: you are free to change and redistribute it.\n");
        printf("There is NO WARRANTY, to the extent permitted by law.\n");
        return 0;
    }

    // Otherwise, treat argv[1] as a filename
    const char *input_path = argv[1];
    char *source = load_file(input_path);
    if (!source) {
        fprintf(stderr, "wpy+.exe: failed to load file: %s\n", input_path);
        return 1;
    }

    // Initialize lexer with source buffer
    set_source(source);   // just call it, don’t re‑implement it


    // 1. Lexing + debug
    printf("Lexing...\n");
    Token tokens[1024];
    int token_count = 0;
    Token tok;
    do {
        tok = next_token();
        printf("Token: %d (%s)\n", tok.type, tok.lexeme ? tok.lexeme : ""); // debug
        tokens[token_count++] = tok;
    } while (tok.type != TOKEN_EOF);

    // 2. Parsing + Interpiling + debug
    printf("Parsing...\n");
    ASTNode *ast = parse(tokens, token_count);
    if (!ast) {
        printf("Parser returned NULL — nothing to run.\n");
    } else {
        printf("AST built successfully:\n");
        print_ast(ast, 0);   // dump the AST tree
        run_program(ast);
        free_ast(ast);
    }

    free(source);
    return 0;
}
