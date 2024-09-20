#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <time.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <unistd.h>

enum Type {
    UNKNOWN = 0,
    REG = 1,
    DR,
    RUN,
    LNK
};

time_t getTime(char* filename) {
    struct stat result;
    if(stat(filename, &result)==0) {
        return result.st_mtime;
    }
    return 0;
}


char* getH_M(time_t time) {
    struct tm* timeinfo;
    char* buffer = (char*) calloc(32, sizeof(char));
    timeinfo = localtime(&time);
    strftime(buffer,32,"%H:%M",timeinfo);
    return buffer;
}

char* getDay(time_t time) {
    struct tm* timeinfo;
    char* buffer = (char*) calloc(32, sizeof(char));
    timeinfo = localtime(&time);
    strftime(buffer,32,"%d",timeinfo);
    return buffer;
}

char* getMonth(time_t time) {
    struct tm* timeinfo;
    char* buffer = (char*) calloc(32, sizeof(char));
    timeinfo = localtime(&time);
    strftime(buffer,32,"%m",timeinfo);
    int month = atoi(buffer);
    free(buffer);
    switch (month)
    {
    case 1:
        return "Jan";
    case 2:
        return "Feb";
    case 3:
        return "Mar";
    case 4:
        return "Apr";
    case 5:
        return "May";
    case 6:
        return "Jun";
    case 7:
        return "Jul";
    case 8:
        return "Aug";
    case 9:
        return "Sep";
    case 10:
        return "Oct";
    case 11:
        return "Nov";
    case 12:
        return "Dec";
    
    default:
        return "What";
    }
}

char* getPermissions(char* filename) {
    struct stat file_stat;
    if (stat(filename, &file_stat) < 0) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    char* permissions = (char*) calloc(10, sizeof(char));
    if (!permissions) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    permissions[0] = (S_ISDIR(file_stat.st_mode)) ? 'd' : '-';
    permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';

    return permissions;
}

int cmp(const void* s1, const void* s2)
{
    const char** a = (const char**) s1;
    const char** b = (const char**) s2;
    char* str1 = calloc(strlen(*a), sizeof(char));
    strcpy(str1, *a);
    int len1 = strlen(str1);
    if (str1[0] == '.' && len1 > 2) {
        memmove(str1, str1 + 1, len1 - 1);
        str1[len1 - 1] = 0;
    }
    char* str2 = calloc(strlen(*b), sizeof(char));
    strcpy(str2, *b);
    int len2 = strlen(str2);
    if (str2[0] == '.' && len2 > 2) {
        memmove(str2, str2 + 1, len2 - 1);
        str2[len2 - 1] = 0;
    }
    for (int i = 0; i < len1; ++i) {
        if (str1[i] > 64 && str1[i] < 91) {
            str1[i] += 32;
        }
    }
    for (int i = 0; i < len2; ++i) {
        if (str2[i] > 64 && str2[i] < 91) {
            str2[i] += 32;
        }
    }
     
    int res = strcmp(str1, str2);
    free(str1);
    free(str2);
    return res;
}

enum Type getFileType(const char *fileName) {
    struct stat fileStat;
    char buff[1024];
    
    if(lstat(fileName, &fileStat) < 0) {
        return -1;
    }
    if(S_ISLNK(fileStat.st_mode)) {
        return LNK; // символическая ссылка
    }
    else if(S_ISDIR(fileStat.st_mode)) {
        return DR; // директория
    } else if((fileStat.st_mode & S_IXUSR) || (fileStat.st_mode & S_IXGRP) || (fileStat.st_mode & S_IXOTH)) {
        return RUN; // исполняемый файл
    }  else if(S_ISREG(fileStat.st_mode)) {
        return REG; // обычный файл
    } else {
        return UNKNOWN; // другой тип файла
    }
}

long getSize(char *fileName) {
    struct stat fileStat;
    if(lstat(fileName, &fileStat) < 0) {
        return -1;
    }
    return fileStat.st_size;
}

int getNumOfLinks(char* fileName) {
    struct stat fileStat;
    if(stat(fileName, &fileStat) < 0) {
        perror("Error in stat");
        return -1;
    }
    return fileStat.st_nlink;
}

char* getGroup(char* fileName) {
    struct stat fileStat;
    if(stat(fileName, &fileStat) < 0) {
        perror("Error in stat");
        return "";
    }
    struct group *grp = getgrgid(fileStat.st_uid);

    if(grp == NULL) {
        perror("Error in getgrpuid");
        return "";
    }
    return grp->gr_name;
}

char* getOwner(char* fileName) {
    struct stat fileStat;
    if(stat(fileName, &fileStat) < 0) {
        perror("Error in stat");
        return "";
    }
    struct passwd *pw = getpwuid(fileStat.st_uid);
    if(pw == NULL) {
        perror("Error in getpwuid");
        return "";
    }
    return pw->pw_name;
}

void setColor(enum Type type) {
    if (type == REG) {
	    printf("\033[1;31m");
    }
    else if (type == DR) {
	    printf("\033[1;34m");
    }
    else if (type == RUN) {
	    printf("\033[1;32");
    }
    else if (type == LNK) {
	    printf("\033[1;36");
    }
    else {
        printf("\033[0m");
    }
}

char* getLinkPath(char* fileName) {
    char *real_path;
    real_path = (char*) calloc(256, sizeof(char));
    ssize_t len = readlink(fileName, real_path, 256 * sizeof(char));
    if (len == -1) {
        perror("readlink");
        free(real_path);
        return "";
    }
    real_path[len] = '\0';
    return real_path;
}

int getColorCode(char* fileName) {
    enum Type type = getFileType(fileName);
    if (type == DR) {
	    return 34;
    }
    if (type == RUN) {
	    return 32;
    }
    if (type == LNK) {
	    return 36;
    }
    return 0;
}

int getStrMasMaxLen(char** mas, int n) {
    int max = 0;
    for (int i = 0; i < n; ++i) {
        if (strlen(mas[i]) > max) {
            max = strlen(mas[i]);
        }
    }
    return max;
}

int getDigitsCount(int a) {
    int digitsCount = 1;
    int d = 10;
    while (a > 10) {
        a = a / 10;
        digitsCount++;
    }
    return digitsCount;
}

int getIntMasMaxLen(long int* mas, int n) {
    int max = 0;
    for (int i = 0; i < n; ++i) {
        if (getDigitsCount(mas[i]) > max) {
            max = getDigitsCount(mas[i]);
        }
    }
    return max;
}

int getFullBlocksCount(char* directory) {
    struct dirent *de;
    DIR *dr = opendir(directory);
    int total = 0;
    while ((de = readdir(dr)) != NULL) {
        struct stat st;
        stat(de->d_name, &st);
        total += st.st_blocks;
    }
    return total;
}

void print(char** names, int namesCount, char* directory) {
    for (int i = 0; i < namesCount; ++i) {
        int code = getColorCode(names[i]);
        if (!(directory[0] == '.' && directory[1] == '\0')) {
            char* pos = strstr(names[i], directory);
            memmove(pos, pos + strlen(directory) + 1, strlen(names[i]) + 1);
        }
        printf("\033[1;%dm%s\033[0m  ", code, names[i]);
    }
    printf("\n");
}

void printL(char** names, int namesCount, char* directory) {
    int linkNums[namesCount];
    int maxLinkLen = 0;
    char* groups[namesCount];
    int maxGroupLen = 0;
    char* owners[namesCount];
    int maxOwnerLen = 0;
    int sizes[namesCount];
    int maxSizeLen = 0;
    long int fullSize = 0;
    for (int i = 0; i < namesCount; ++i) {
        struct stat st;
        struct stat sta;
        lstat(directory, &sta);
        lstat(names[i], &st);\
        fullSize += st.st_blocks;
        linkNums[i] = getNumOfLinks(names[i]);
        if (getDigitsCount(linkNums[i]) > maxLinkLen) {
            maxLinkLen = getDigitsCount(linkNums[i]);
        }
        groups[i] = getGroup(names[i]);
        if (strlen(groups[i]) > maxGroupLen) {
            maxGroupLen = strlen(groups[i]);
        }
        owners[i] = getOwner(names[i]);
        if (strlen(owners[i]) > maxOwnerLen) {
            maxOwnerLen = strlen(owners[i]);
        }
        sizes[i] = getSize(names[i]);
        if (getDigitsCount(sizes[i]) > maxSizeLen) {
            maxSizeLen = getDigitsCount(sizes[i]);
        }
    }
    printf("total %d\n", fullSize / 2);
    for (int i = 0; i < namesCount; ++i) {
        char* permissions = getPermissions(names[i]);
        char* month = getMonth(getTime(names[i]));
        char* day = getDay(getTime(names[i]));
        char* time = getH_M(getTime(names[i]));
        printf("%s %*d %*s %*s %*d %s %s %s ", permissions, maxLinkLen, linkNums[i], maxGroupLen, groups[i],
                maxOwnerLen, owners[i], maxSizeLen, sizes[i], month, day, time);
        int code = getColorCode(names[i]);
        char* link = NULL;
        if (getFileType(names[i]) == LNK) {
            link = getLinkPath(names[i]);
        }
        if (!(directory[0] == '.' && directory[1] == '\0')) {
            char* pos = strstr(names[i], directory);
            memmove(pos, pos + strlen(directory) + 1, strlen(names[i]) + 1);
        }
        printf("\033[1;%dm%s\033[0m", code, names[i]);
        if (link != NULL) {
            char* copyLink = (char*) calloc(64, sizeof(char));
            strcpy(copyLink, directory);
            strcat(copyLink, link);
            printf(" -> \033[1;%dm%s\033[0m", getColorCode(copyLink), link);
            free(link);
            free(copyLink);
        }
        printf("\n");
        free(permissions);
        free(day);
        free(time);
    }
}

bool isFind(char** mas, int n, char* str) {
    for (int i = 0; i < n; ++i) {
        if (strcmp(str, mas[i]) == 0) {
            return true;
        }
        if (mas[i] == NULL) {
            break;
        }
    }
    return false;
}

int main(int argc, char** argv) {
    char c;
    bool isA;
    bool isL;
    while ((c = getopt(argc, argv, "la")) != -1)
    {
        switch (c)
        {
        case 'a':
            isA = true;
            break;

        case 'l':
            isL = true;
        
        }
    }
    char* directory = (char*) calloc(256, sizeof(char));
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            strcpy(directory, argv[i]);
            break;
        }
    }

    if (directory[0] == 0) {
        directory[0] = '.';
    }
    DIR *dir;
    struct dirent *ent;
    char* names[64];
    for (int i = 0; i < 64; ++i) {
        names[i] = (char*) calloc(256, sizeof(char));
    }
    int curName = 0;
    if ((dir = opendir (directory)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (isA || ((ent->d_name[0] != '.'))) {
                if (directory[0] == '.' && directory[1] == '\0') {
                    strcpy(names[curName], ent->d_name);
                }
                else {
                    char* direct = (char*) calloc(256, sizeof(char));
                    strcpy(direct, directory);
                    direct = strcat(strcat(direct, "/"), ent->d_name);
                    strcpy(names[curName], direct);
                    free(direct);
                }
                curName++;
            }
        }
        closedir (dir);
    } 
    else {
        perror ("");
        for (int i = 0; i < 64; ++i) {
            free(names[i]);
        }
        return 1;
    }

    qsort(names, curName, sizeof(char *), cmp);
    if (isL) {
        printL(names, curName, directory);
    }
    else {
        print(names, curName, directory);
    }

    for (int i = 0; i < 64; ++i) {
        free(names[i]);
    }
    free(directory);
    return 0;
}
