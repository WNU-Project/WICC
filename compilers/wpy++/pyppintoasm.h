#ifndef PYPPINTOASM_H
#define PYPPINTOASM_H

#include <stdio.h>
#include "parser.h"

void generate_asm(ASTNode *ast, FILE *out);

#endif
