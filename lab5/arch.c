#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>


typedef struct {
    char name[32];
    long int size;
    mode_t mode;
} File;

int openArch(char* name) {
    int fd = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fprintf(stderr, "Error open file");
        exit(1);
    }
    return fd;
}

bool inputFileToArch(char* archName, char* fileName) {
    int fdArch = openArch(archName);
    int fdFile = open(fileName, O_RDONLY);
    if (fdFile < 0) {
        close(fdFile);
        return false;
    }
    File file;
    strcpy(file.name, fileName);
    file.size = lseek(fdFile, 0, SEEK_END);
    struct stat file_stat;
    stat(fileName, &file_stat);
    file.mode = file_stat.st_mode;
    lseek(fdFile, 0, SEEK_SET);
    lseek(fdArch, 0, SEEK_END);
    if (!write(fdArch, &file, sizeof(File))) {
        fprintf(stderr, "Write error\n");
    }
    char buffer[file.size];
    int i;
    while ((i = read(fdFile, buffer, file.size)) > 0) {
        write(fdArch, buffer, i);
    }
    close(fdFile);
    return true;
}

void removeFileFromArch(char* archName, char* fileName, int fullsize) {
    int fdArch = openArch(archName);
    char archBuffer[fullsize];
    File* files = (File*) calloc(16, sizeof(File));
    int i = 0;
    while (1) {
        File file;
        int res = read(fdArch, &file, sizeof(File));
        if (res == 0) {
            break;
        }
        if (strcmp(fileName, file.name)) {
            files[i] = file;
            i++;
            if (file.size != 0) {
                char buffer[file.size];
                if (!(read(fdArch, buffer, file.size))) {
                    fprintf(stderr, "Read error\n");
                    exit(1);
                }
                strcat(archBuffer, buffer);
            }
            continue;
        }
        lseek(fdArch, file.size, SEEK_CUR);
    }
    close(fdArch);
    remove(archName);
    fdArch = openArch(archName);
    for (int j = 0; j < i; ++j) { 
        if (files[j].size == -1) {
            break;
        }
        write(fdArch, &files[j], sizeof(File));
        write(fdArch, &archBuffer, files[j].size);
    }
    close(fdArch);
}

bool extractFileFromArch(char* archName, char* fileName) {
    int fdArch = openArch(archName);
    int fullsize = 0;
    bool isFound = false;
    while (1) {
        File file;
        int res = read(fdArch, &file, sizeof(File));
        if (res == 0) {
            break;
        }
        if (strcmp(fileName, file.name)) {
            fullsize += file.size;
            lseek(fdArch, file.size, SEEK_CUR);
            continue;
        }
        int fdFile = open(file.name, O_RDWR | O_CREAT);
        chmod(file.name, file.mode);
        if (fdFile < 0) {
            fprintf(stderr, "Extract error");
            exit(1);
        }
        isFound = true;
        char buffer[file.size];
        if (file.size != 0) {
            if (!read(fdArch, buffer, file.size)) {
                fprintf(stderr, "Read error\n");
                exit(1);
            }
            if (!write(fdFile, buffer, file.size)) {
                fprintf(stderr, "Write error\n");
                exit(1);
            }
        }
        close(fdFile);
    }
    if (!isFound) {
        printf("No such file\n");
        return false;
    }
    removeFileFromArch(archName, fileName, fullsize);
    return true;
}

bool extractArch(char* archName) {
    int fdArch = openArch(archName);
    
    while (1) {
        File file;
        int res = read(fdArch, &file, sizeof(File));
        if (res == 0) {
            break;
        }
        if (access(file.name, F_OK) != -1) {
            chmod(file.name, 0777);
        }
        int fdFile = open(file.name, O_RDWR | O_CREAT);
        chmod(file.name, file.mode);
        if (fdFile < 0) {
            fprintf(stderr, "Extract error\n");
            return false;
        }
        if (file.size > 0) {
        char buffer[file.size];
            if (!read(fdArch, buffer, file.size)) {
                fprintf(stderr, "Read error\n");
                return false;
            }
            if (!write(fdFile, buffer, file.size)) {
                fprintf(stderr, "Write error\n");
                return false;
            }
        }
        close(fdFile);
    }
    printf("Extract successfull\n");
    close(fdArch);
    return true;
}

void status(char* archName) {
    int fdArch = openArch(archName);
    bool isAny = false;
    while (1) {
        File file;
        int res = read(fdArch, &file, sizeof(File));
        if (res == 0) {
            break;
        }
        isAny = true;
        printf("%s - %dB\n", file.name, file.size);
        lseek(fdArch, file.size, SEEK_CUR);
    }
    if (!isAny) {
        printf("Archive is empty\n");
    }
    close(fdArch);
}

int main(int argc, char** argv) {
    if (argc == 2) {
        if (strcmp(argv[1], "-h")) {
            fprintf(stderr, "Few arguments\n");
            return 1;
        }
        else {
            printf("Archivator:\n");
            printf("To create a archive, enter %s arch_name -i first_file\n", argv[0]);
            printf("To add file to archive, enter %s arch_name -i file_name\n", argv[0]);
            printf("To extract file, enter %s arch_name -e file_name\nThis file won't be in archive anymore\n\n", argv[0]);
            printf("To extract all archive, enter %s arch_name -e\n", argv[0]);
            printf("To show status of archive, enter %s arch_name -s\n", argv[0]);
            return 0;
        }
    }
    if (argc < 3) {
        fprintf(stderr, "Few arguments\n");
        return 1;
    }
    if (argc == 3 && (strcmp(argv[2], "-e") && strcmp(argv[2], "-s"))) {
        fprintf(stderr, "Few arguments\n");
        return 1;
    }
    int c;
    bool isI = false;
    bool isE = false;
    bool isS = false;
    while ((c = getopt(argc, argv, "ies")) != -1)
    {
        switch (c)
        {
        case 'i':
            isI = true;
            break;

        case 'e':
            isE = true;
            break;
        case 's':
            isS = true;
            break;
        }
    }
    char archName[32];
    strcpy(archName, argv[2]);
    if (isI) {
        if (!inputFileToArch(archName, argv[3])) {
            fprintf(stderr, "Incorrect file\n");
            return 1;
        }
        else {
            printf("Success\n");
        }
        return 0;
    }
    if (isE) {
        if (argc == 3) {
            extractArch(archName);
        }
        else {
            if(extractFileFromArch(archName, argv[3])) {
                printf("Extract successfull\n");
            }
            else {
                printf("Failure\n");
            }
        }
        return 0;
    }
    if (isS) {
        status(archName);
        return 0;
    }

    return 0;
}
