/*
    Exercise: Multithreaded Counter with Mutex

    This program creates multiple threads that each increment a shared
    counter ("mails") a large number of times. Because all threads modify
    the same shared variable, a mutex is used to ensure that only one
    thread at a time can update the counter. This prevents data races and
    guarantees a correct final value. Each thread reports when it starts
    and finishes, and the program prints the final total after all
    threads have joined.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

pthread_mutex_t mutex;
pthread_cond_t cond;
int mails = 0;

void* routine(void* arg){
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 100000000; i++)
        mails++;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

static void error_handler(void *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    pthread_t thread[NUM_THREADS];

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread[i], NULL, &routine, NULL) != 0)
            error_handler("pthread_create");
        printf("Thread %d has started.\n", i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(thread[i], NULL) != 0)
                error_handler("pthread_join");
            
        printf("Thread %d has finished execution.\n", i);
    }

    pthread_mutex_destroy(&mutex);
    printf("Number of mails: %d\n", mails);
    
    return 0;
}
