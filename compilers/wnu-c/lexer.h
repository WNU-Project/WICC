#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"

// Initialize lexer with a source file path. Returns 0 on success.
int lexer_init(const char *path);

// Get next token. Returns a Token with allocated lexeme (caller should free lexeme if not NULL).
Token lexer_next(void);

// Free any resources used by the lexer (call at program end).
void lexer_free(void);

// Initialize lexer with source code
int lexer();
#endif // LEXER_H