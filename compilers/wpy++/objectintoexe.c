#include <stdio.h>
#include <stdlib.h>

int object_to_exe(const char *obj_path, const char *exe_path) {
    char cmd[1024];
    // Use gcc as the linker
    snprintf(cmd, sizeof(cmd), "\"C:\\msys64\\mingw64\\bin\\gcc.exe\" -o %s %s", exe_path, obj_path);
    printf("Linking: %s\n", cmd);
    return system(cmd);
}
