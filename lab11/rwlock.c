#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

pthread_mutex_t mtx;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;


int mas[10];
int curMax = 0;
bool isRead = true;

void* pthread_func_read(void* arg) {
    pthread_mutex_lock(&mtx);
    printf("%d %d\n", curMax, *(int*)arg);
    while (isRead) {
        pthread_rwlock_wrlock(&rwlock);
    }
    printf("My tid - %x\n", pthread_self());
    printf("Current max for mas - %d\n", curMax - 1);
    for (int i = 0; i < curMax; ++i) {
        printf("%d ", mas[i]);
    } 
    printf("\n");
    printf("\n");
    isRead = true;
    pthread_rwlock_unlock(&rwlock);
    pthread_mutex_unlock(&mtx);
    pthread_exit(NULL);
}

void* pthread_func_write(void* arg) {
    pthread_mutex_lock(&mtx);
    while (curMax < 10) {
        int ret = 0;
        while (isRead != true) {
            pthread_rwlock_rdlock(&rwlock);
        }
        mas[curMax] = curMax;
        curMax++;
        printf("Write ended\n");
        isRead = false;
        pthread_rwlock_unlock(&rwlock);
        pthread_mutex_unlock(&mtx);
    }
    pthread_exit(NULL);
}



int main() {
    pthread_mutex_init(&mtx, NULL);
    pthread_t threadsRead[10];
    pthread_t threadWrite;
    pthread_create(&threadWrite, NULL, pthread_func_write, NULL);
    for (int i = 0; i < 10; ++i) {
        pthread_create(&threadsRead[i], NULL, pthread_func_read, (void*)&i);
        usleep(50);
    }
    for (int i = 0; i < 10; ++i) {
        pthread_join(threadsRead[i], NULL);
    } 
    pthread_mutex_destroy(&mtx);
    return 0;
}
