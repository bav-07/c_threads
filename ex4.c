#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NTHREADS 12

int cont = 0;
pthread_mutex_t mutex;

void *PrintHello(void *arg) {
    printf("I'm thread %d and my value is %d\n", *(int *) arg, cont);
    // sleep(*(int *) arg);
    return arg;    
}

void *callFunction(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        cont++;
        PrintHello(arg);
        pthread_mutex_unlock(&mutex);
    }
    return arg;
}

int main(int argc, char const *argv[])
{
    pthread_mutex_init(&mutex, NULL);
    pthread_t threads[NTHREADS];
    int rc, t;
    int *taskids[NTHREADS];
    for (t = 0; t < NTHREADS; t++) {
        
        taskids[t] = (int *) malloc(sizeof(int));
        *taskids[t] = t;
        printf("Creating thread %d\n", t);
        rc = pthread_create(&threads[t], NULL, callFunction, (void *) taskids[t]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        // if (t == 1) {
        //     void *retval;
        //     int status = pthread_join(threads[1], &retval);
        //     if (status) {
        //         printf("ERROR; unable to join threads.");
        //         exit(-1);
        //     }
        //     int *intReturnVal = (int *) retval;
        //     printf("Return value: %d. Return status: %d\n", *intReturnVal, status);
        // }
    }
    pthread_exit(NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}
