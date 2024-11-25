#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    int res;
    if (mkfifo("fifo", 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            exit(1);
        }
    }
    switch((res = fork())) {
        case -1: {
            int err = errno;
            fprintf(stderr, "Error:%s (%d)\n", strerror(err), err);
            break;
        }
        case 0: {
            char* str = "Example string\n";
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            printf("Передаем %s", str);
            printf("%d: Текущее время: %02d:%02d:%02d\n", getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);
            int fd = open("fifo", O_WRONLY);
            write(fd, str, strlen(str) + 1);
            close(fd);
            printf("-----------------------------\n");
            break;
        }
        default: {
            sleep(5);
            char buf[4];
            int len = 0;
            int waitRes = 0;
            int fd = open("fifo", O_RDONLY);
            char* str = calloc(32, sizeof(char));
            while((len = read(fd, buf, sizeof(buf))) != 0) {
                strncat(str, buf, sizeof(buf));
            }
            printf("Получено %s", str);
            free(str);
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            printf("Текущее время: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
            break;
        }
    }
    unlink("fifo");
    return 0;
}
