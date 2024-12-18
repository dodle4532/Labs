#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mtx;
pthread_cond_t cond;

int mas[10];
int curMax = 0;

void* pthread_func_read(void* arg) {
    int i = *(int*)arg;
    pthread_mutex_lock(&mtx);
    while (curMax <= i) { // Ждем когда пора читать
        pthread_cond_wait(&cond, &mtx);
    }
    //printf("%d---%d\n", curMax, i);
    printf("My tid - %x\n", pthread_self());
    printf("Current max for mas - %d\n", curMax - 1);
    for (int i = 0; i < curMax; ++i) {
        printf("%d ", mas[i]);
    } 
    printf("\n");
    printf("\n");
    sleep(1);
    pthread_mutex_unlock(&mtx);
    pthread_exit(NULL);
}

void* pthread_func_write(void* arg) {
    while(curMax < 10) {
        pthread_mutex_lock(&mtx);
        mas[curMax] = curMax;
        curMax++;
        printf("Write ended\n");
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mtx);
        sleep(1);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_t threadsRead[10];
    pthread_t threadWrite;
    pthread_create(&threadWrite, NULL, pthread_func_write, NULL);
    for (int i = 0; i < 10; ++i) {
        pthread_create(&threadsRead[i], NULL, pthread_func_read, (void*)&i);
        usleep(50); // гарантирует, что в arg все правильно передастся
    }
    pthread_join(threadWrite, NULL);
    for (int i = 0; i < 10; ++i) {
        pthread_join(threadsRead[i], NULL);
    } 
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);
    return 0;
}
