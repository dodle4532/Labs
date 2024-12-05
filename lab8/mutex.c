#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mtx;

int mas[10];
int curMax = 0;

void* pthread_func_read(void* arg) {
    pthread_mutex_lock(&mtx);
    printf("My tid - %x\n", pthread_self());
    printf("Current max for mas - %d\n", curMax - 1);
    for (int i = 0; i < curMax; ++i) {
        printf("%d ", mas[i]);
    } 
    printf("\n");
    printf("\n");
    pthread_mutex_unlock(&mtx);
    pthread_exit(NULL);
}

void* pthread_func_write(void* arg) {
    pthread_mutex_lock(&mtx);
    mas[curMax] = curMax;
    curMax++;
    pthread_mutex_unlock(&mtx);
    pthread_exit(NULL);
}



int main() {
    pthread_mutex_init(&mtx, NULL);
    pthread_t threadsRead[10];
    pthread_t threadWrite;
    for (int i = 0; i < 10; ++i) {
        pthread_create(&threadWrite, NULL, pthread_func_write, NULL);
        usleep(200000);
        pthread_create(&threadsRead[i], NULL, pthread_func_read, NULL);
    }
    for (int i = 0; i < 10; ++i) {
        pthread_join(threadsRead[i], NULL);
    } 
    pthread_mutex_destroy(&mtx);
    return 0;
}
