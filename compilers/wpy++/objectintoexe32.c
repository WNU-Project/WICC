// objectintoexe32.c
#include <stdio.h>
#include <stdlib.h>

// Link a 32-bit object file into a 32-bit Windows executable.
// obj_path: path to the .o file (e.g. "out32.o")
// exe_path: path to the .exe file to create (e.g. "out32.exe")
// Returns 0 on success, nonzero on failure.
int object_to_exe32(const char *obj_path, const char *exe_path) {
    char cmd[1024];
    // Allow override with environment variable GCC32_PATH
    const char *env_gcc = getenv("GCC32_PATH");
    const char *msys_gcc = "C:\\msys64\\mingw32\\bin\\gcc.exe";
    const char *gcc_cmd = NULL;

    if (env_gcc && env_gcc[0] != '\0') {
        gcc_cmd = env_gcc;
    } else {
        FILE *f = fopen(msys_gcc, "r");
        if (f) { fclose(f); gcc_cmd = msys_gcc; }
        else gcc_cmd = "i686-w64-mingw32-gcc"; // fallback to PATH
    }

    // Build command: no -m32 needed when using a 32-bit GCC
    snprintf(cmd, sizeof(cmd),
             "%s \"%s\" -o \"%s\" -lmsvcrt -Wl,-subsystem,console",
             gcc_cmd, obj_path, exe_path);

    printf("Linking (32-bit) using: %s\n", gcc_cmd);
    printf("Command: %s\n", cmd);
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Linking failed for 32-bit build (code %d) (THIS IS EXPECTED DUE OF THIS YOU CAN'T MAKE THIS 32 BITS [ACCORDING TO MY COMPUTER FOR TESTING])\n", ret);
        return 1;
    }

    printf("Executable created: %s\n", exe_path);
    return 0;
}
