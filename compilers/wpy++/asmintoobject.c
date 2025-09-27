#include "asmintoobject.h"
#include <stdio.h>
#include <stdlib.h>

int assemble_to_object(const char *asm_path, const char *obj_path) {
    char cmd[1024];

    // Always emit .o object file
    // On Windows with MinGW, use -f win64
    // On Linux, you’d switch this to -f elf64
    snprintf(cmd, sizeof(cmd), "nasm -f win64 %s -o %s", asm_path, obj_path);

    printf("Assembling: %s\n", cmd);
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Assembly failed with code %d\n", ret);
        return 1;
    }

    printf("Object file created: %s\n", obj_path);
    return 0;
}
