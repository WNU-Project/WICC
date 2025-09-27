// asmintoobject32.h
#ifndef ASMINTOOBJECT32_H
#define ASMINTOOBJECT32_H

// Assemble a 32-bit NASM source file into a 32-bit object file.
// asm_path: path to the .asm file (e.g. "out32.asm")
// obj_path: path to the .o file to create (e.g. "out32.o")
// Returns 0 on success, nonzero on failure.
int assemble_to_object32(const char *asm_path, const char *obj_path);

#endif // ASMINTOOBJECT32_H
