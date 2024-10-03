#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

void func() {
    printf("I'm atexit 1 for process %d\n", getpid());
}
void func2() {
    printf("I'm atexit 2 for process %d\n", getpid());
}

// Так я проверял отработку SIGTERM, нажимал Ctrl+C и сначала заходило сюда - потом убиваю его. В общем случае 2 функции избыточны
void handlerINT(int sig) {
    //kill(getpid(), 15);
    printf("Signal %d recieved\n", sig);
}

void handlerTERM(int sig) {
    printf("Signal %d recieved\n", sig);
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    int res = 0;
    atexit(func);
    signal(SIGINT, handlerINT);
    signal(SIGTERM, handlerTERM);
    switch (res = fork()) {
        case -1:
            int err = errno;
            fprintf(stderr, "Fork error : %s (%d)\n", strerror(err), err);
            break;
        case 0:
            atexit(func2);
            sleep(3);
            printf("[CHILD]I'm child of %d, my pid is %d\n", getppid(), getpid());
            break;
        default:
            //signal(SIGINT, handler);
            int result;
            wait(&result);
            printf("[PARENT]I'm parent of %d, my pid id %d, my parent pid is %d\n", res, getpid(), getppid());
            printf("[PARENT] Child exit code %d\n", WEXITSTATUS(result));
            break;
    }
    return 0;
}
