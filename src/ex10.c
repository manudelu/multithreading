/*
    Exercise: Producer–Consumer Problem Using Pthreads and Semaphores

    This program implements the classic producer–consumer problem using
    POSIX threads, mutexes, and semaphores. A fixed-size buffer is shared
    between producer and consumer threads.

    The exercise addresses three key synchronization problems:
      1) Managing shared memory access using a mutex to protect the buffer
         and the shared counter.
      2) Preventing producers from adding items when the buffer is full
         using an "empty" semaphore.
      3) Preventing consumers from removing items when the buffer is empty
         using a "full" semaphore.

    Producers generate data and place it into the buffer, while consumers
    remove and process data. This demonstrates safe coordination between
    multiple threads accessing shared resources.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_THREADS 3

pthread_mutex_t mutexBuffer;
sem_t empty;
sem_t full;

int buffer[10];
int count = 0;

void* producer(void* arg){
    while(1){
        // Produce
        int x = rand()%100;
        //sleep(1);

        sem_wait(&empty);
        pthread_mutex_lock(&mutexBuffer);

        // Add to the buffer
        buffer[count] = x;
        count++;
        
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg){
    while(1) {
        int y;

        sem_wait(&full);
        pthread_mutex_lock(&mutexBuffer);

        // Remove from the buffer
        y = buffer[count - 1];
        count--;

        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&empty);

        // Consume
        printf("Got %d\n", y);
        sleep(1);
    }
    return NULL;
}

static void error_handler(void *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    pthread_t thread[NUM_THREADS];

    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&empty, 0, 10);
    sem_init(&full, 0, 0);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (i > 0 == 0) {
            if (pthread_create(&thread[i], NULL, &producer, NULL) != 0)
                error_handler("pthread_create");
        }
        else {
            if (pthread_create(&thread[i], NULL, &consumer, NULL) != 0)
                error_handler("pthread_create");
        }
        
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(thread[i], NULL) != 0)
                error_handler("pthread_join");
    }

    pthread_mutex_destroy(&mutexBuffer);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
