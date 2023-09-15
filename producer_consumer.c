#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <fcntl.h>

#define THREAD_NUM 16
#define BUFFER_CAPACITY 10

int buffer[BUFFER_CAPACITY];
int itemsInBuffer = 0;

pthread_cond_t condBuffer;

void *produce(void *args) {
    sem_t *semaphore = sem_open("/login2", 0);
    sem_wait(semaphore);
    // sleep(1);
    while (itemsInBuffer >= 10) {
        printf("(%d) Waiting to produce...\n", *(int*)args);
        pthread_cond_wait(&condBuffer, &semaphore);
    }
    buffer[itemsInBuffer] = 1;
    itemsInBuffer++;
    printf("(%d) Produced. Items in Buffer: %d\n", *(int*)args, itemsInBuffer);
    sem_post(semaphore);

}

void *consume(void *args) {
    sem_t *semaphore = sem_open("/login2", 0);
    sem_wait(semaphore);
    // sleep(1);
    while (itemsInBuffer == 0) {
        printf("(%d) Waiting to consume...\n", *(int*)args);
        pthread_cond_wait(&condBuffer, &semaphore);
    }
    buffer[itemsInBuffer] = 0;
    itemsInBuffer--;
    printf("(%d) Consumed. Itmes in Buffer: %d\n", *(int*)args, itemsInBuffer);
    sem_post(semaphore);

}

int main(int argc, char const *argv[])
{
    sem_unlink("/login2");
    pthread_cond_init(&condBuffer, NULL);
    pthread_t producers[THREAD_NUM];
    pthread_t consumers[THREAD_NUM];
    // the second parameter for sem_init depends on how many processes you have (not threads!) If you have only one process pass 0, otherwise, pass 1
    // the third parameter is the sem_t *semProd = sem_open(SEM_PRODUCER_FNAME, IPC_CREAT, 0660, 0);
    sem_t *semaphore = sem_open("/login2", O_CREAT, 0660, 100);
    if (semaphore == SEM_FAILED) {
        perror("sem_open/producer");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        int* a = malloc(sizeof(int));
        int* b = malloc(sizeof(int));
        *a = i;
        *b = i;
        if (pthread_create(&producers[i], NULL, &produce, a) != 0) {
            perror("Failed to create thread\n");
        }
        if (pthread_create(&consumers[i], NULL, &consume, b) != 0) {
            perror("Failed to create thread\n");
        }
    }
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(producers[i], NULL) != 0) {
            perror("Failed to join thread\n");
        }
        if (pthread_join(consumers[i], NULL) != 0) {
            perror("Failed to join thread\n");
        }
    } 
    sem_unlink("/login2");
    pthread_cond_destroy(&condBuffer);
    return 0;
}
