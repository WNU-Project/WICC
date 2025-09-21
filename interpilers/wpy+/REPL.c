#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "interpiler.h"

void run_repl(void) {
    printf("Python+ 1.0.1 (WNU build, %s %s) [WICC interpiler 64-bit] on win32\n", __DATE__, __TIME__);
    printf("Type \"help\", \"manifesto\", or \"license\" for more information.\n");

    char line[1024];
    while (1) {
        printf(">>> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        // Trim newline
        line[strcspn(line, "\r\n")] = 0;

        if (strcmp(line, "exit") == 0) break;

        if (strcmp(line, "help") == 0) {
            printf("Python+ help: use 'manifesto' for philosophy, 'license' for GPL info, 'exit' to quit.\n");
            continue;
        }
        if (strcmp(line, "manifesto") == 0) {
            printf("Python+ is part of the WNU movement: every program is a declaration.\n");
            continue;
        }
        if (strcmp(line, "license") == 0) {
            printf("Python+ is licensed under GPLv3. See <https://gnu.org/licenses/gpl.html>.\n");
            continue;
        }

        // Tokenize and parse the line
        Token tokens[256];
        int count = lex_line(line, tokens);   // you’ll need a helper like this
        ASTNode *ast = parse(tokens, count);
        if (ast) {
            interpret(ast);
            free_ast(ast);
        } else {
            printf("Parse error.\n");
        }
    }
}
