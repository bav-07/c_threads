#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <fcntl.h>

#define THREAD_NUM 16

void *routine(void *args) {
    printf("(%d) Waiting in the login queue\n", *(int*)args);
    sem_t *semaphore = sem_open("/login2", 0);
    sem_wait(semaphore);
    printf("(%d) Logged in\n", *(int*)args);
    sleep(1);
    printf("(%d) Logged out\n", *(int*)args);
    sem_post(semaphore);

}

int main(int argc, char const *argv[])
{
    sem_unlink("/login2");
    pthread_t th[THREAD_NUM];
    pthread_t th2[THREAD_NUM];
    // the second parameter for sem_init depends on how many processes you have (not threads!) If you have only one process pass 0, otherwise, pass 1
    // the third parameter is the sem_t *semProd = sem_open(SEM_PRODUCER_FNAME, IPC_CREAT, 0660, 0);
    sem_t *semaphore = sem_open("/login2", O_CREAT, 0660, 2);
    if (semaphore == SEM_FAILED) {
        perror("sem_open/producer");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        int* a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th[i], NULL, &routine, a) != 0) {
            perror("Failed to create thread\n");
        }
    }
    for (i = 0; i < THREAD_NUM; i++) {
        int* a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th2[i], NULL, &routine, a) != 0) {
            perror("Failed to create thread\n");
        }
        
    }
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread\n");
        }
        if (pthread_join(th2[i], NULL) != 0) {
            perror("Failed to join thread\n");
        }
    } 
    sem_unlink("/login2");
    return 0;
}
