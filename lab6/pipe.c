#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
//дочерний и родительскиц
int main(int argc, char **argv) {
    (void)argc; (void)argv;

    int res = 0;
    int pipedesc[2];
    int pipeRes = pipe(pipedesc);
    if(pipeRes != 0) {
        int err = errno;
        fprintf(stderr, "Error:%s (%d)\n", strerror(err), err);
        return EXIT_FAILURE;
    }
    switch((res = fork())) {
        case -1: {
            int err = errno;
            fprintf(stderr, "Error:%s (%d)\n", strerror(err), err);
            break;
        }
        case 0: {
            char* str = "Example string\n";
            close(pipedesc[0]);
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            printf("Передаем %s", str);
            printf("%d: Текущее время: %02d:%02d:%02d\n", getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);
            write(pipedesc[1], str, strlen(str) + 1);
            close(pipedesc[1]);
            printf("-----------------------------\n");
        }
        default: {
            sleep(5);
            char buf[4];
            int len = 0;
            int waitRes = 0;
            close(pipedesc[1]);
            char* str = calloc(16, sizeof(char));
            while((len = read(pipedesc[0], buf, sizeof(buf))) != 0) {
                strcat(str, buf);
            }
            printf("Получено %s", str);
            free(str);
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            printf("Текущее время: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
            close(pipedesc[0]);
            break;
        }
    }
    return 0;
}
