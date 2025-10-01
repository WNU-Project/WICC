#include <stdio.h>
#include <stdlib.h>

int gra_object_to_exe(const char *obj_file, const char *exe_file, int is32bit) {
    char cmd[512];

    if (is32bit) {
        snprintf(cmd, sizeof(cmd), "C:\\msys64\\mingw64\\bin\\gcc.exe -m32 outgra.obj -o %s -lgdi32 -luser32 -Wl,-subsystem,windows", exe_file);
        printf("Linking (gra, 32-bit): %s\n", cmd);
    } else {
        snprintf(cmd, sizeof(cmd), "C:\\msys64\\mingw64\\bin\\gcc.exe outgra.obj -o %s -lgdi32 -luser32 -Wl,-subsystem,windows", exe_file);
        printf("Linking (gra, 64-bit): %s\n", cmd);
    }

    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Linking failed for %s\n", exe_file);
        return 1;
    }

    printf("Executable created: %s\n", exe_file);
    return 0;
}
