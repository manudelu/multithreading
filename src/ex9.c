/*
    Exercise: Login Queue Using Semaphores

    This program simulates a login system where only a limited number of
    users can be logged in simultaneously. A semaphore is used to control
    access to the shared resource (login slots).

    Multiple threads represent users trying to log in. Each thread waits
    on the semaphore before logging in, stays logged in for a short random
    time, and then logs out, releasing the semaphore. This demonstrates
    how semaphores can be used to limit concurrent access and manage
    queues in multithreaded applications.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 16

sem_t semaphore;

void* routine(void* arg){
    printf("(%d) Waiting in the login queue... \n", *(int*)arg);
    sem_wait(&semaphore);
    printf("(%d) Logged in. \n", *(int*)arg);
    sleep(rand()%5 + 1);
    printf("(%d) Logged out. \n", *(int*)arg);
    sem_post(&semaphore);
    free(arg);
    return NULL;
}

static void error_handler(void *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    pthread_t thread[NUM_THREADS];
    sem_init(&semaphore, 0, 4);

    for (int i = 0; i < NUM_THREADS; i++) {
        int *a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&thread[i], NULL, &routine, a) != 0)
            error_handler("pthread_create");
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(thread[i], NULL) != 0)
                error_handler("pthread_join");
    }
    sem_destroy(&semaphore);

    return 0;
}
