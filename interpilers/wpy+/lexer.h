#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include "tokens.h"

char *load_file(const char *path);
void set_source(const char *src);
Token next_token(void);

#endif
