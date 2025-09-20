// lexer.h
#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"

char *load_file(const char *path);
void set_source(const char *src);
Token next_token(void);

#endif
