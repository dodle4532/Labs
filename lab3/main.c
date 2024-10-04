#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void func() {
    printf("I'm atexit 1 for process %d\n", getpid());
}
void func2() {
    printf("I'm atexit 2 for process %d\n", getpid());
}

void handler(int sig) {
    printf("Signal %d recieved, pid - %d\n", sig, getpid());
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    int res = 0;
    atexit(func);
    signal(SIGINT, handler);
    struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));
    sigterm_action.sa_handler = &handler;
    sigterm_action.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &sigterm_action, NULL);
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
            int result;
            wait(&result);
            printf("[PARENT]I'm parent of %d, my pid id %d, my parent pid is %d\n", res, getpid(), getppid());
            printf("[PARENT] Child exit code %d\n", WEXITSTATUS(result));
            break;
    }
    return 0;
}
