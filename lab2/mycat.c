#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
    char c;
    bool isN, isB, isE;
    while ((c = getopt(argc, argv, "nbE")) != -1)
    {
        switch (c)
        {
        case 'n':
            isN = true;
            break;
        case 'b':
            isB = true;
            break;
        case 'E':
            isE = true;
            break;
        }
    }
    char* filePath = (char*) calloc(256, sizeof(char));
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            strcpy(filePath, argv[i]);
            break;
        }
    }
    if (filePath[0] == 0) {
        fprintf(stderr, "No file\n");
        free(filePath);
        return 1;
    }

    FILE *file;
    file = fopen(filePath, "r");
    if (file) {
        int count = 1;
        bool isNewRow = true;
        char* string = (char*) calloc(256, sizeof(char));
        int index = 0;
        while ((c = getc(file)) != EOF) {
            if (c == 13) {
                continue;
            }
            string[index] = c;
            index++;
            if (c == '\n') {
                if (isE) {
                    string[index - 1] = 36;
                    string[index] = '\n';
                }
                if (isN || (isB && (string[0] != 13 && string[0] != 10))) {
                    fprintf(stdout, "     %d  ", count);
                    count++;
                }
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
        return 1;
    }

    free(filePath);
    return 0;
}
