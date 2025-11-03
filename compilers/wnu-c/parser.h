#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

// Initialize parser on input file. Returns 0 on success.
int parser_init(const char *path);

// Parse translation unit and return AST root (caller frees with ast_free).
Ast *parser_parse_translation_unit(void);

// Free parser resources.
void parser_free(void);

#endif // PARSER_H
