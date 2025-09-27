// asmintoobject32.c
#include <stdio.h>
#include <stdlib.h>

// Assemble a 32-bit NASM source file into a 32-bit object file.
// Returns 0 on success, nonzero on failure.
int assemble_to_object32(const char *asm_path, const char *obj_path) {
    char cmd[512];
    // Use NASM with -f win32 for 32-bit COFF object
    snprintf(cmd, sizeof(cmd), "nasm -f win32 %s -o %s", asm_path, obj_path);

    printf("Assembling (32-bit): %s\n", cmd);
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Assembly failed for 32-bit build (code %d)\n", ret);
        return 1;
    }

    printf("Object file created: %s\n", obj_path);
    return 0;
}
