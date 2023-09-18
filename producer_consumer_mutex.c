#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <fcntl.h>

#define THREAD_NUM 2
#define BUFFER_CAPACITY 10

int buffer[BUFFER_CAPACITY];
int itemsInBuffer = 0;

pthread_mutex_t mutexBuffer;
pthread_cond_t condBuffer;

sem_t *producerSem;
sem_t *consumerSem;

// print the buffer
void printBuffer() {
    printf("[");
    for (int i = 0; i < BUFFER_CAPACITY; i++) {
        printf(" %d ", buffer[i]);
    }
    printf("]\n\n");
}

// add an item to the buffer
void *produce(void *args) {
    while(1) {
        sem_wait(producerSem);
        pthread_mutex_lock(&mutexBuffer);
        // only add the item if the buffer is not full
        // while (itemsInBuffer == BUFFER_CAPACITY) {
        //     printf("(%d P) Waiting to produce...\n", *(int*)args);
        //     // wait for the condition variable to be satisfied before exiting this while loop
        //     pthread_cond_wait(&condBuffer, &mutexBuffer);
        // }
        // set the next item in the buffer to 1 to indicate presence
        buffer[itemsInBuffer] = 1;
        // add one to the num of items in the buffer
        itemsInBuffer++;
        printf("(%d P) Produced. Items in Buffer: %d\n", *(int*)args, itemsInBuffer);
        printBuffer();
        pthread_mutex_unlock(&mutexBuffer);
        // pthread_cond_broadcast(&condBuffer);
        // free(args);
        sem_post(consumerSem);
    }
    
}

void *consume(void *args) {
    while (1) {
        sem_wait(consumerSem);
        pthread_mutex_lock(&mutexBuffer);
        // sleep(1);
        // while (itemsInBuffer == 0) {
        //     printf("(%d C) Waiting to consume...\n", *(int*)args);
        //     pthread_cond_wait(&condBuffer, &mutexBuffer);
        // }
        buffer[itemsInBuffer - 1] = 0;
        itemsInBuffer--;
        printf("(%d C) Consumed. Items in Buffer: %d\n", *(int*)args, itemsInBuffer);
        printBuffer();
        pthread_mutex_unlock(&mutexBuffer);
        // pthread_cond_broadcast(&condBuffer);
        // free(args);
        sem_post(producerSem);
    }
}

int main(int argc, char const *argv[])
{
    sem_unlink("/produce");
    sem_unlink("/consume");

    producerSem = sem_open("/produce", O_CREAT, 0660, BUFFER_CAPACITY);
    consumerSem = sem_open("/consume", O_CREAT, 0660, 0);

    pthread_mutex_init(&mutexBuffer, NULL);
    pthread_cond_init(&condBuffer, NULL);
    pthread_t producers[THREAD_NUM];
    pthread_t consumers[THREAD_NUM];

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
    pthread_mutex_destroy(&mutexBuffer);
    pthread_cond_destroy(&condBuffer);

    sem_unlink("/produce");
    sem_unlink("/consume");

    return 0;
}
