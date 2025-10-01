#include <stdio.h>
#include <stdlib.h>

// Assemble an assembly file into an object file for the gra (graphics) path
int gra_asm_to_object(const char *asm_file, const char *obj_file, int is32bit) {
    char cmd[512];

    if (is32bit) {
        snprintf(cmd, sizeof(cmd), "nasm -f win32 %s -o outgra.obj", asm_file);
        printf("Assembling (gra, 32-bit): %s\n", cmd);
    } else {
        snprintf(cmd, sizeof(cmd), "nasm -f win64 %s -o outgra.obj", asm_file);
        printf("Assembling (gra, 64-bit): %s\n", cmd);
    }

    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Assembling failed for %s\n", asm_file);
        return 1;
    }

    printf("Object file created: %s\n", obj_file);
    return 0;
}
