#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

char* addr;
int shmid;

void handle_sigterm(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        printf("\nEnd of sending\n");
        shmdt(addr);
        shmctl(shmid, IPC_RMID, NULL);
        exit(0);
    }
}

int main() {
    key_t key = ftok("shm", 1);
    shmid = shmget(key, 64, IPC_CREAT | 0666 | IPC_EXCL);
    if (shmid < 0) {
        int err = errno;
        printf("shmget %s (%d)\n", strerror(err), err);
        return 1;
    }

    addr = (char*)shmat(shmid, NULL, 0);
    printf("Sending start, pid - %d\n", getpid());
    if (addr == (void*)-1) {
        int err = errno;
        printf("shmat %s (%d)\n", strerror(err), err);
        return 1;
    }
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handle_sigterm;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    while(1) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(addr, "Pid of sender - %d, Time of sender - %02d:%02d:%02d", getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);
        sleep(3);
    }
    return 0;
}
