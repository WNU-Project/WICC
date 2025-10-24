/*
* WNU C Compiler (wnu-c)
* Main entry point
* C11 Language Standard (more standards coming soon)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("WNU C Compiler (wnu-c) - C11 Standard\n");
    printf("Copyright (C) 2025 WNU Project\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");

    if (argc < 2) {
        printf("wnu-c: \033[1;31mfatal error:\033[0m no input files\n");
        printf("Usage: wnu-c <source_file.c> [options]\n");
        return 1;
    }

    const char *input_file = argv[1];
    printf("Compiling %s...\n", input_file);

    // Placeholder for actual compilation logic
    // ...

    printf("Compilation finished successfully.\n");
    return 0;
}