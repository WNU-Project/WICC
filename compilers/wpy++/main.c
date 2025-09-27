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

// Forward declarations from lexer.c
void init_lexer(const char *source);
Token next_token(void);
void free_token(Token *tok);

// Forward declaration from codegen.c
int generate_asm_to_file(ASTNode *ast, const char *out_path);

static void show_help() {
    printf("Usage: wpy++.exe <source_file.pypp> [options]\n");
    printf("Options:\n");
    printf("  --help, -h       Show this help message\n");
    printf("  --version, -v    Show version information\n");
}

static void show_version() {
    printf("WICC wpy++.exe 1.0.0 (Python++ Compiler)\n");
    printf("Copyright (C) 2025 WNU Project\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
}

// Map token types to readable names
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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m no arguments provided\n");
        show_help();
        return 1;
    }

    // Handle known flags
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        show_help();
        return 0;
    }
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        show_version();
        return 0;
    }

    // Otherwise, treat argv[1] as a source file path
    const char *filepath = argv[1];
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m cannot open file '%s'\n", filepath);
        return 1;
    }

    // Read file into memory
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(fp);
        return 1;
    }
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);

    // Run lexer
    init_lexer(buffer);

    // Collect tokens into an array
    int capacity = 64;
    int count = 0;
    Token *tokens = malloc(sizeof(Token) * capacity);

    Token tok;
    do {
        tok = next_token();
        if (count >= capacity) {
            capacity *= 2;
            tokens = realloc(tokens, sizeof(Token) * capacity);
        }
        tokens[count++] = tok;
    } while (tok.type != TOKEN_EOF);

    printf("Lexing complete. %d tokens.\n", count);

    // --- Token dump ---
    printf("Token dump:\n");
    for (int i = 0; i < count; i++) {
        printf("  [%2d] %-15s Lexeme='%s' (line %d, col %d)\n",
               i, token_type_name(tokens[i].type),
               tokens[i].lexeme, tokens[i].line, tokens[i].column);
    }

    // Parse tokens into AST
    ASTNode *ast = parse(tokens, count);
    printf("Parsing complete. AST:\n");
    print_ast(ast, 0);

    // Generate assembly file
    char outpath[512];
    snprintf(outpath, sizeof(outpath), "%s.asm", filepath);
    if (generate_asm_to_file(ast, outpath) == 0) {
        printf("Assembly written to out.asm\n");
    } else {
        fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m failed to generate assembly\n");
    }

    if (assemble_to_object("out.asm", "out.o") == 0) {
        printf("Object file created: out.o\n");
    }

    if (assemble_to_object("out.asm", "out.o") == 0) {
        if (object_to_exe("out.o", "out.exe") == 0) {
            printf("Executable created: out.exe\n");
        }
    }
    if (generate_asm32(ast) == 0) {
            printf("32-bit Assembly written to out32.asm\n");
            if (assemble_to_object32("out32.asm", "out32.o") == 0) {
                printf("32-bit Object file created: out32.o\n");
                if (object_to_exe32("out32.o", "out32.exe") == 0) {
                    printf("32-bit Executable created: out32.exe\n");
                }
            } else {
                fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m failed to assemble 32-bit object\n");
            }

        } else {
            fprintf(stderr, "wpy++.exe: \033[1;31mfatal error:\033[0m failed to generate 32-bit assembly\n");
        }

    // Cleanup
    for (int i = 0; i < count; i++) {
        free_token(&tokens[i]);
    }
    free(tokens);
    free(buffer);
    free_ast(ast);

    return 0;
}
