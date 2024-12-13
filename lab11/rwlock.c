#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_rwlock_t rwlock;

int mas[10];
int curMax = 0;

void* pthread_func_read(void* arg) {
    int i = *(int*)arg;
    pthread_rwlock_rdlock(&rwlock);
    while (curMax <= i) { // Если еще не пора читать, то скинем мьютекс и попробуем еще раз
        pthread_rwlock_unlock(&rwlock);
        usleep(1000);
        pthread_rwlock_rdlock(&rwlock);

    }
    sleep(1);
    printf("My tid - %x\n", pthread_self());
    printf("Current max for mas - %d\n", curMax - 1);
    for (int i = 0; i < curMax; ++i) {
        printf("%d ", mas[i]);
    } 
    printf("\n");
    printf("\n");
    pthread_rwlock_unlock(&rwlock);
    pthread_exit(NULL);
}

void* pthread_func_write(void* arg) {
    while(curMax < 10) {
        pthread_rwlock_wrlock(&rwlock);
        mas[curMax] = curMax;
        curMax++;
        printf("Write ended\n");
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    pthread_exit(NULL);
}



int main() {
    pthread_rwlock_init(&rwlock, NULL);
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
    pthread_rwlock_destroy(&rwlock);
    return 0;
}
