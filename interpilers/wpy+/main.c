#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Check for arguments
    if (argc < 2) {
        printf("wpy+.exe: \033[1;31mfatal error:\033[0m no arguments provided \033[1;31mError Code: 1\033[0m\n");
        printf("Usage: wpy+.exe <source_file.pyp> [options]\n");
        return 1;
    }

    // Handle flags first
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "help") == 0   || strcmp(argv[1], "h") == 0) {
        printf("wpy+.exe: Python+ Interpreter\n");
        printf("Usage: wpy+.exe <source_file.pyp>\n");
        printf("Options:\n");
        printf("  --help, -h    Show this help message\n");
        printf("  --version, -v Show version information\n");
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        printf("wpy+.exe version 0.1.0\n");
        printf("Python+ Interpreter (WNU Project)\n");
        return 0;
    }

    // Otherwise, treat argv[1] as a filename
    const char *input_path = argv[1];
    FILE *fp = fopen(input_path, "rb");
    if (!fp) {
        perror("wpy+.exe: failed to open input file");
        return 1;
    }

    // For now, just confirm success
    printf("Opened file: %s\n", input_path);

    fclose(fp);
    return 0;
}
