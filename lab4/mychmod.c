#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

bool isPlus = false;
char* path;

/*
S_IRUSR: Owner Read permission (Owner Read)
S_IWUSR: Owner Write permission (Owner Write)
S_IXUSR: Owner Execute permission (Owner Execute)
S_IRGRP: Group member read permission (Group Read)
S_IWGRP: Group member write permission (Group Write)
S_IXGRP: Execute permission for group members (Group Execute)
S_IROTH: Read permission for other users (Others Read)
S_IWOTH: Write permission for other users (Others Write)
S_IXOTH: Execute permission for other users (Others Execute)
*/

mode_t getMode(char first, char second) {
    if (first == 'u' && second == 'r') {
        return S_IRUSR;
    }
    if (first == 'u' && second == 'w') {
        return S_IWUSR;
    }
    if (first == 'u' && second == 'x') {
        return S_IXUSR;
    }
    if (first == 'g' && second == 'r') {
        return S_IRGRP;
    }
    if (first == 'g' && second == 'w') {
        return S_IWGRP;
    }
    if (first == 'g' && second == 'x') {
        return S_IXGRP;
    }
    if (first == 'o' && second == 'r') {
        return S_IROTH;
    }
    if (first == 'o' && second == 'w') {
        return S_IWOTH;
    }
    if (first == 'o' && second == 'x') {
        return S_IXOTH;
    }
    return 0;
}

void doChmod(char* fileName, mode_t* modes) {
    int i = 1;
    mode_t final_mode = modes[0];
    while (modes[i] != 0) {
        final_mode |= modes[i];
        i++;
    }
    struct stat file_stat;
    stat(fileName, &file_stat);
    if (isPlus) chmod(fileName, file_stat.st_mode | final_mode);
    else chmod(fileName, file_stat.st_mode & ~final_mode);
}

mode_t* getPermisson(char* string) {
    mode_t* modes = (mode_t*) calloc(16, sizeof(mode_t));
    int index = 0;
    char* first = (char*) calloc(4, sizeof(char));
    char* second = (char*) calloc(4, sizeof(char));
    bool isFirst = true;
    int j = 0;
    for (int i = 0; i < strlen(string); ++i) {
        if (string[i] == '=') {
            char* str = (char*)calloc(16, sizeof(char));
            strcpy(str, "-rwx");
            mode_t* mode = getPermisson(str);
            doChmod(path, mode);
            free(mode);
            free(str);
            isPlus = true;
            isFirst = false;
            j = 0;
            continue;
        }
        if (string[i] == '+') {
            isPlus = true;
            isFirst = false;
            j = 0;
            continue;
        }
        if (string[i] == '-') {
            isFirst = false;
            j = 0;
            continue;
        }
        if (isFirst) {
            first[j] = string[i];
            j++;
        }
        else {
            second[j] = string[i];
            j++;
        }
    }
    if (strlen(second) == 0) {
        fprintf(stderr, "Incorrect arguments\n");
        free(first);
        free(second);
        free(modes);
        exit(1);
    }
    if (strlen(first) == 0) {
        first[0] = 'u';
        first[1] = 'g';
        first[2] = 'o';
    }
    for (int i = 0; i < strlen(first); ++i) {
        for (int j = 0; j < strlen(second); ++j) {
            modes[index] = getMode(first[i], second[j]);
            if (modes[index] == 0) {
                fprintf(stderr, "Error in getMode");
                exit(1);
            }
            index++;
        }
    }
    modes[index] = 0;
    free(first);
    free(second);
    return modes;
}

mode_t** getModes(char* string) {
    mode_t** modes = (mode_t**)calloc(4, sizeof(mode_t*));
    int index = 0;
    char* str = (char*)calloc(8, sizeof(char));
    for (int i = 0; i < 3; ++i) {
        str[0] = string[i];
        int n = atoi(str);
        switch (i)
        {
        case 0:
            str[0] = 'u';
            break;
        case 1:
            str[0] = 'g';
            break;
        case 2:
            str[0] = 'o';
            break;
        
        default:
            break;
        }
        str[1] = '+';
        switch (n)
        {
        case 0:
            memset(str, 0, sizeof(str));
            continue;
            break;
        case 1:
            str[2] = 'x';
            break;
        case 2:
            str[2] = 'w';
            break;
        case 3:
            str[2] = 'x';
            str[3] = 'w';
            break;
        case 4:
            str[2] = 'r';
            break;
        case 5:
            str[2] = 'x';
            str[3] = 'r';
            break;
        case 6:
            str[2] = 'r';
            str[3] = 'w';
            break;
        case 7:
            str[2] = 'r';
            str[3] = 'w';
            str[4] = 'x';
            break;
        
        default:
            break;
        }
        modes[index] = getPermisson(str);
        index++;
        memset(str, 0, sizeof(str));
    }
    free(str);
    modes[index] = NULL;
    return modes;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Incorrect arguments\n");
        return 1;
    }
    char* argument = argv[1];
    path = argv[2];

    if (isdigit(argument[0])) {
        if (strlen(argument) != 3) {
            fprintf(stderr, "Incorrect arguments\n");
            return 1;
        }
        char* str = (char*)calloc(16, sizeof(char));
        strcpy(str, "-rwx");
        mode_t* mode = getPermisson(str);
        doChmod(path, mode);
        free(mode);
        free(str);

        mode_t** modes = getModes(argument);
        for (int i = 0; i < 3; ++i) {
            doChmod(path, modes[i]);
        }
        for (int i = 0; i < 3; ++i) {
            free(modes[i]);
        }
        free(modes);
        return 0;
    }

    mode_t* modes = getPermisson(argument);
    doChmod(path, modes);
    free(modes);
    return 0;
}
