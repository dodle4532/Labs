#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
    bool isBash;
    char* substr = (char*) calloc(64, sizeof(char));
    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        return 1;
    }
    if (argc > 3) {
        fprintf(stderr, "Too many arguments\n");
        return 1;
    }
    if (argc < 3) {
        isBash = true;
    }
    strcat(substr, argv[1]);
    if (isBash) {
        char line[256];
        while (fgets(line, sizeof(line), stdin) != NULL) {
            if (strstr(line, substr))
                fprintf(stdout, "%s", line);
        }
        return 0;
    }
    char* filePath = (char*) calloc(256, sizeof(char));
    strcat(filePath, argv[2]);

    FILE *file;
    file = fopen(filePath, "r");
    if (file) {
        int count = 1;
        bool isNewRow = true;
        char* string = (char*) calloc(256, sizeof(char));
        int index = 0;
        char c;
        while ((c = getc(file)) != EOF) {
            if (c == 13) {
                continue;
            }
            string[index] = c;
            index++;
            if (c == '\n') {
                if (strstr(string, substr))
                    fprintf(stdout, "%s", string);
                if (string[0] != 0) {
                    free(string);
                    string = (char*) calloc(256, sizeof(char));
                }
                index = 0;
            }
        }
        if (string[0] != 0) {
            fprintf(stdout, "%s", string);
            free(string);
        }
        fclose(file);
    }
    else {
        perror("Incorrect path");
        free(filePath);
        free(substr);
        return 1;
    }

    free(filePath);
    free(substr);
    return 0;
}
