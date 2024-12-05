#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main() {
    key_t key = ftok("shm", 1);
    int shmid = shmget(key, 64, 0666);

    if (shmid < 0) {
        int err = errno;
        printf("shmget %s (%d)\n", strerror(err), err);
        return 1;
    }   
    int semid = semget(key, 1, 0666);
    if (semid < 0) {
        int err = errno;
        printf("semget %s (%d)\n", strerror(err), err);
        return 1;
    }
    char* addr = (char*)shmat(shmid, NULL, 0);
    if (addr == (void*)-1) {
        int err = errno;
        printf("shmat %s (%d)\n", strerror(err), err);
        return 1;
    }
    if (!strcmp(addr, "")) {
        printf("Empty string\n");
        shmctl(shmid, IPC_RMID, NULL);
    }
    else {
        printf("Recieved string:\n");
        struct sembuf sop = {0, 1, 0};
        semop(semid, &sop, 1);
        printf("%s\n", addr);
    }
    shmdt(addr);
    return 0;
}
