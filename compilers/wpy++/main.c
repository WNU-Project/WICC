#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "parser.h"
#include "asmintoobject.h"
#include "objectintoexe.h"
#include "pyppintoasm.h"
#include "pyppintoasm32.h"
#include "asmintoobject32.h"
#include "objectintoexe32.h"
#include "gra_pyppintoasm.h"
#include "gra_asmintoobject.h"
#include "gra_objectintoexe.h"

static void show_help() {
    printf("Usage: wpy++.exe <source_file.pypp> [options]\n");
    printf("Options:\n");
    printf("  --help, -h       Show this help message\n");
    printf("  --version, -v    Show version information\n");
    printf("  --win32          Build as Win32 GDI app (graphics pipeline) PLEASE REMEMBER THIS IS A WIP (WORK IN PROGRESS)\n");
    printf("  --no32           Skip 32-bit build (only produce 64-bit executable)\n");
}

static void show_version() {
    printf("WICC wpy++.exe 1.0.0 (Python++ Compiler)\n");
    printf("Copyright (C) 2025 WNU Project\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
}

static const char* token_type_name(TokenType t) {
    switch (t) {
        case TOKEN_IDENTIFIER:     return "IDENTIFIER";
        case TOKEN_INT_LITERAL:    return "INT_LITERAL";
        case TOKEN_STRING_LITERAL: return "STRING_LITERAL";
        case TOKEN_CHAR_LITERAL:   return "CHAR_LITERAL";
        case TOKEN_FUNC:           return "FUNC";
        case TOKEN_RETURN:         return "RETURN";
        case TOKEN_SUCCESS:        return "SUCCESS";
        case TOKEN_FAILURE:        return "FAILURE";
        case TOKEN_IF:             return "IF";
        case TOKEN_ELSE:           return "ELSE";
        case TOKEN_WHILE:          return "WHILE";
        case TOKEN_FOR:            return "FOR";
        case TOKEN_INCLUDE:        return "INCLUDE";
        case TOKEN_LBRACE:         return "LBRACE";
        case TOKEN_RBRACE:         return "RBRACE";
        case TOKEN_LPAREN:         return "LPAREN";
        case TOKEN_RPAREN:         return "RPAREN";
        case TOKEN_SEMICOLON:      return "SEMICOLON";
        case TOKEN_COMMA:          return "COMMA";
        case TOKEN_DOT:            return "DOT";
        case TOKEN_PLUS:           return "PLUS";
        case TOKEN_MINUS:          return "MINUS";
        case TOKEN_STAR:           return "STAR";
        case TOKEN_SLASH:          return "SLASH";
        case TOKEN_ASSIGN:         return "ASSIGN";
        case TOKEN_EQ:             return "EQ";
        case TOKEN_NEQ:            return "NEQ";
        case TOKEN_GT:             return "GT";
        case TOKEN_LT:             return "LT";
        case TOKEN_GTE:            return "GTE";
        case TOKEN_LTE:            return "LTE";
        case TOKEN_EOF:            return "EOF";
        default:                   return "UNKNOWN";
    }
}

// Forward declarations from lexer.c
void init_lexer(const char *source);
Token next_token(void);
void free_token(Token *tok);

// Forward declaration from codegen.c (console)
int generate_asm_to_file(ASTNode *ast, const char *out_path);

int main(int argc, char *argv[]) {
    int is_win32_mode = 0;
    int attempt_32 = 1; // whether to try producing a 32-bit build
    const char *filepath = NULL;

    if (argc < 2) {
        fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m no arguments provided\n");
        show_help();
        return 1;
    }

    // Parse args: first non-flag is the source file; flags can appear anywhere
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            show_help();
            return 0;
        } else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            show_version();
            return 0;
        } else if (strcmp(argv[i], "--win32") == 0) {
            is_win32_mode = 1;
        } else if (strcmp(argv[i], "--no32") == 0) {
            attempt_32 = 0;
        } else {
            filepath = argv[i];
        }
    }

    if (!filepath) {
        fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m no source file provided\n");
        return 1;
    }

    // Read file
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m cannot open file '%s'\n", filepath);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buffer = (char*)malloc(size + 1);
    if (!buffer) { fclose(fp); fprintf(stderr, "Memory allocation failed\n"); return 1; }
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);

    // Lex
    init_lexer(buffer);
    int capacity = 64, count = 0;
    Token *tokens = (Token*)malloc(sizeof(Token) * capacity);
    Token tok;
    do {
        tok = next_token();
        if (count >= capacity) {
            capacity *= 2;
            tokens = (Token*)realloc(tokens, sizeof(Token) * capacity);
        }
        tokens[count++] = tok;
    } while (tok.type != TOKEN_EOF);

    printf("Lexing complete. %d tokens.\n", count);

    // Dump tokens (optional)
    printf("Token dump:\n");
    for (int i = 0; i < count; i++) {
        printf("  [%2d] %-15s Lexeme='%s' (line %d, col %d)\n",
               i, token_type_name(tokens[i].type),
               tokens[i].lexeme, tokens[i].line, tokens[i].column);
    }

    // Parse
    ASTNode *ast = parse(tokens, count);
    printf("Parsing complete. AST:\n");
    print_ast(ast, 0);

    int status = 0;

    if (is_win32_mode) {
        // Graphics pipeline (Win32 GDI)
        if (gra_generate_asm_to_file(ast) != 0) {
            fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m failed to generate graphics assembly\n");
            status = 1;
            goto cleanup;
        }
        printf("Assembly written to outgra.asm (Win32 GDI)\n");

        // IMPORTANT: use .obj for NASM COFF64 on Windows
        if (gra_asm_to_object("outgra.asm", "outgra.obj", 0) != 0) {
            fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m failed to assemble graphics object\n");
            status = 1;
            goto cleanup;
        }
        printf("Object file created: outgra.obj\n");

        if (gra_object_to_exe("outgra.obj", "outgra.exe", 0) != 0) {
            fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m failed to link graphics executable\n");
            status = 1;
            goto cleanup;
        }
        printf("Executable created: outgra.exe\n");
    } else {
        // Console pipeline
        char outpath[512];
        snprintf(outpath, sizeof(outpath), "%s.asm", filepath);

        if (generate_asm_to_file(ast, "out.asm") != 0) {
            fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m failed to generate assembly\n");
            status = 1;
            goto cleanup;
        }
        printf("Assembly written to out.asm\n");

        if (assemble_to_object("out.asm", "out.o") != 0) {
            fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m failed to assemble object\n");
            status = 1;
            goto cleanup;
        }
        printf("Object file created: out.o\n");

        if (object_to_exe("out.o", "out.exe") != 0) {
            fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m failed to link executable\n");
            status = 1;
            goto cleanup;
        }
        printf("Executable created: out.exe\n");

        // Optional: attempt 32-bit (your machine may fail — skip if --no32 given)
        if (attempt_32) {
            if (generate_asm32(ast) == 0) {
                printf("32-bit Assembly written to out32.asm\n");
                if (assemble_to_object32("out32.asm", "out32.o") == 0) {
                    printf("32-bit Object file created: out32.o\n");
                    if (object_to_exe32("out32.o", "out32.exe") == 0) {
                        printf("32-bit Executable created: out32.exe\n");
                    } else {
                        fprintf(stderr, "Linking failed for 32-bit build (expected on some setups)\n");
                    }
                }
            }
        } else {
            printf("Skipping optional 32-bit build (use --no32 to suppress this message)\n");
        }
    }

cleanup:
    for (int i = 0; i < count; i++) free_token(&tokens[i]);
    free(tokens);
    free(buffer);
    free_ast(ast);
    return status;
}
