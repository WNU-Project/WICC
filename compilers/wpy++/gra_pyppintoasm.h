#ifndef GRA_PYPPINTOASM_H
#define GRA_PYPPINTOASM_H

#include "parser.h"

// Generate NASM assembly for Win32 GDI apps
int gra_generate_asm_to_file(ASTNode *ast);

#endif // GRA_PYPPINTOASM_H
