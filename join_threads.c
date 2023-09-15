#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 100

pthread_mutex_t mutex;

void *roll_dice(void *arg) {
    pthread_mutex_lock(&mutex);
    int value = (rand() % 6) + 1;
    int *result = malloc(sizeof(int));
    *result = value;
    printf("Thread %d result: %p\n", *(int *)arg, result);
    pthread_mutex_unlock(&mutex);
    return (void *) result;
}

int main(int argc, char const *argv[])
{
    pthread_mutex_init(&mutex, NULL);
    int *res;
    srand(time(NULL));
    int *taskids[ITERATIONS];
    pthread_t th[ITERATIONS];
    for (int i = 0; i < ITERATIONS ; i++ ) {
        taskids[i] = (int *) malloc(sizeof(int));
        *taskids[i] = i;
        if (pthread_create(&th[i], NULL, &roll_dice, (void *) taskids[i]) != 0) {
            return 1;
        }
    }
    for (int i = 0; i < ITERATIONS ; i++ ) {
        if (pthread_join(th[i], (void**) &res) != 0) {
            return 2;
        }
        printf("Thread %d Roll: %d\n", i, *res);
    }
    
    // printf("Main result: %p\n", res);
    free(res);
    pthread_mutex_destroy(&mutex);
    return 0;
}
