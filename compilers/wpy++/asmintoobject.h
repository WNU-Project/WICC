#ifndef ASMINTOOBJECT_H
#define ASMINTOOBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Assemble a NASM-style .asm file into an object file.
 *
 * This does not link — it only produces the object file.
 *
 * @param asm_path  Path to the input assembly file (e.g. "out.asm").
 * @param obj_path  Path to the output object file (e.g. "out.o" or "out.obj").
 * @return 0 on success, nonzero on failure.
 */
int assemble_to_object(const char *asm_path, const char *obj_path);

#ifdef __cplusplus
}
#endif

#endif // ASMINTOOBJECT_H
