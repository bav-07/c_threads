#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h> // Include for sleep function

#define BUFFER_SIZE 10

sem_t empty;    // Semaphore to count empty slots in the buffer
sem_t full;     // Semaphore to count filled slots in the buffer
sem_t mutex;    // Mutex to protect access to the buffer
int buffer[BUFFER_SIZE];
int in = 0;     // Index for the next item to be inserted
int out = 0;    // Index for the next item to be removed
bool shouldTerminate = false; // Flag to indicate when threads should terminate

void *producer(void *arg) {
    int item = 0;
    while (!shouldTerminate) { // Check termination condition
        
        // Produce an item
        item++;

        // Wait for an empty slot in the buffer
        sem_wait(&empty);

        // Acquire the buffer mutex
        sem_wait(&mutex);
        
        // Insert the item into the buffer
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;

        // Release the buffer mutex
        sem_post(&mutex);

        // Signal that the buffer is now full
        sem_post(&full);
        
        printf("Produced: %d\n", item);
    }
    
    printf("Producer thread has finished.\n");
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int item;
    while (!shouldTerminate) { // Check termination condition
        
        // Wait for a filled slot in the buffer
        sem_wait(&full);

        // Acquire the buffer mutex
        sem_wait(&mutex);

        // Remove an item from the buffer
        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        
        // Release the buffer mutex
        sem_post(&mutex);

        // Signal that an empty slot is now available
        sem_post(&empty);
        
        printf("Consumed: %d\n", item);
    }
    
    printf("Consumer thread has finished.\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t producer_thread, consumer_thread;
    
    // Initialize semaphores
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);
    
    // Create producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    
    // Let the threads run for a while
    sleep(5); // Sleep for 5 seconds
    
    // Set the termination flag to true
    shouldTerminate = true;
    
    // Join threads
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    // Clean up semaphores
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    
    return 0;
}