#ifndef PYPPINTOASM32_H
#define PYPPINTOASM32_H

#include "parser.h"

// Generate 32-bit Windows NASM assembly from the parsed AST.
// Writes to "out32.asm" and returns 0 on success, nonzero on error.
int generate_asm32(ASTNode *ast);

#endif // PYPPINTOASM32_H
