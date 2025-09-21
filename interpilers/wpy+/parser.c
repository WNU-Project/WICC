#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#define _POSIX_C_SOURCE 200809L

// Track includes
static int has_pypstdio = 0;

// Utility to create AST nodes
static ASTNode *make_node(ASTNodeType type, const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

// Minimal parser: supports #include <pypstdio> and func main() { ... }
ASTNode *parse(Token *tokens, int token_count) {
    if (token_count < 1) return NULL;

    int i = 0;

    // --- Handle includes ---
    while (i < token_count && tokens[i].type == TOKEN_INCLUDE) {
        // Be permissive about how includes are emitted by the lexer.
        // Possible shapes we accept:
        // 1) TOKEN_INCLUDE with lexeme "pypstdio"
        // 2) TOKEN_INCLUDE with lexeme "<pypstdio>" or '"pypstdio"'
        // 3) TOKEN_INCLUDE followed by TOKEN_LT TOKEN_IDENTIFIER TOKEN_GT
        // 4) TOKEN_INCLUDE followed by TOKEN_IDENTIFIER (no angle brackets)

        int consumed = 0;

        if (tokens[i].lexeme && tokens[i].lexeme[0] != '\0') {
            // Normalize lexeme: trim surrounding <>, '"', or whitespace
            const char *s = tokens[i].lexeme;
            size_t len = strlen(s);
            size_t start = 0, end = len;
            // strip whitespace
            while (start < end && isspace((unsigned char)s[start])) start++;
            while (end > start && isspace((unsigned char)s[end-1])) end--;
            // strip angle brackets or quotes
            if (end - start >= 2 && s[start] == '<' && s[end-1] == '>') { start++; end--; }
            else if (end - start >= 2 && (s[start] == '"' && s[end-1] == '"')) { start++; end--; }

            char buf[128] = {0};
            size_t copy_len = (end - start) < (sizeof(buf)-1) ? (end - start) : (sizeof(buf)-1);
            memcpy(buf, s + start, copy_len);
            buf[copy_len] = '\0';

            if (strcmp(buf, "pypstdio") == 0) {
                has_pypstdio = 1;
            }
            consumed = 1;
            i += 1;

        } else if (i+3 < token_count &&
                   tokens[i+1].type == TOKEN_LT &&
                   tokens[i+2].type == TOKEN_IDENTIFIER &&
                   tokens[i+3].type == TOKEN_GT) {

            if (strcmp(tokens[i+2].lexeme, "pypstdio") == 0) {
                has_pypstdio = 1;
            }
            consumed = 4;
            i += 4; // skip past include <name>

        } else if (i+1 < token_count && tokens[i+1].type == TOKEN_IDENTIFIER) {
            // allow "#include pypstdio" without angle brackets
            if (strcmp(tokens[i+1].lexeme, "pypstdio") == 0) {
                has_pypstdio = 1;
            }
            consumed = 2;
            i += 2;

        } else {
            // Emit helpful diagnostic with surrounding tokens
            fprintf(stderr, "Parse error: malformed #include directive at token %d\n", i);
            fprintf(stderr, "Context tokens:\n");
            for (int k = i; k < i+6 && k < token_count; k++) {
                fprintf(stderr, "  [%d] type=%d lexeme='%s'\n", k, tokens[k].type, tokens[k].lexeme ? tokens[k].lexeme : "(null)");
            }
            return NULL;
        }

        // loop continues to handle multiple leading includes
    }

    // --- Expect func ---
    if (i >= token_count || tokens[i].type != TOKEN_FUNC) {
        fprintf(stderr, "Parse error: expected 'func'\n");
        return NULL;
    }

    // Expect function name
    if (i+1 >= token_count || tokens[i+1].type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parse error: expected function name\n");
        return NULL;
    }

    ASTNode *func = make_node(AST_FUNCTION, tokens[i+1].lexeme);

    // --- Scan body for print/return ---
    for (int j = i+2; j < token_count; j++) {
        // Detect pypstdio.print
        if (tokens[j].type == TOKEN_IDENTIFIER &&
            strcmp(tokens[j].lexeme, "pypstdio") == 0) {

            if (!has_pypstdio) {
                fprintf(stderr, "Semantic error: 'pypstdio' used without #include <pypstdio>\n");
                free_ast(func);
                return NULL;
            }

            if (j+2 < token_count &&
                tokens[j+1].type == TOKEN_DOT &&
                strcmp(tokens[j+2].lexeme, "print") == 0) {

                if (j+4 < token_count &&
                    tokens[j+3].type == TOKEN_LPAREN &&
                    tokens[j+4].type == TOKEN_STRING) {

                    ASTNode *print = make_node(AST_PRINT, tokens[j+4].lexeme);
                    func->children = realloc(func->children, sizeof(ASTNode*) * (func->child_count + 1));
                    func->children[func->child_count++] = print;
                }
            }
        }

        // Handle bare print("...") only if not part of pypstdio.print
        if (tokens[j].type == TOKEN_IDENTIFIER &&
            strcmp(tokens[j].lexeme, "print") == 0) {
            if ((j == 0 || tokens[j-1].type != TOKEN_DOT) &&
                j+2 < token_count &&
                tokens[j+1].type == TOKEN_LPAREN &&
                tokens[j+2].type == TOKEN_STRING) {

                ASTNode *print = make_node(AST_PRINT, tokens[j+2].lexeme);
                func->children = realloc(func->children, sizeof(ASTNode*) * (func->child_count + 1));
                func->children[func->child_count++] = print;
            }
        }

        // Handle return
        if (tokens[j].type == TOKEN_RETURN && j+1 < token_count) {
            ASTNode *ret = make_node(AST_RETURN, tokens[j+1].lexeme);
            func->children = realloc(func->children, sizeof(ASTNode*) * (func->child_count + 1));
            func->children[func->child_count++] = ret;
        }
    }

    return func;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }
    free(node->children);
    free(node->value);
    free(node);
}

void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    switch (node->type) {
        case AST_FUNCTION: printf("Function: %s\n", node->value); break;
        case AST_PRINT:    printf("Print: %s\n", node->value); break;
        case AST_RETURN:   printf("Return: %s\n", node->value); break;
        default:           printf("Node\n"); break;
    }
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], indent + 1);
    }
}
