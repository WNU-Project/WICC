#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("wpy+.exe: \033[1;31mfatal error:\033[0m no arguments provided \033[1;31mError Code: 1\033[0m\n");
        return 1;
    }
    printf("Hello, %s!\n", argv[1]);
    return 0;
}