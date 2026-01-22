/*
    Exercise: Recursive Mutex Demonstration

    This program demonstrates the use of a recursive mutex, which allows
    the same thread to lock the same mutex multiple times without causing
    a deadlock. 

    Each thread locks the mutex multiple times, modifies a shared variable
    (fuel), and then unlocks the mutex the same number of times. Using a
    standard mutex here would cause a deadlock, but a recursive mutex
    allows repeated locking by the same thread.

    This exercise shows how recursive mutexes can be used safely in
    situations where reentrant locking is required.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 8

pthread_mutex_t mutexFuel;
int fuel = 50;

void* routine(void* arg){
    pthread_mutex_lock(&mutexFuel);
    pthread_mutex_lock(&mutexFuel);
    pthread_mutex_lock(&mutexFuel);
    pthread_mutex_lock(&mutexFuel);
    fuel += 50;
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexFuel);
    printf("Incremented fuel to %d\n", fuel);
    return NULL;
}

static void error_handler(void *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    pthread_t thread[NUM_THREADS];
    pthread_mutexattr_t recursiveMutexAttributes;
    pthread_mutexattr_init(&recursiveMutexAttributes);
    pthread_mutexattr_settype(&recursiveMutexAttributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutexFuel, &recursiveMutexAttributes);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread[i], NULL, &routine, NULL) != 0)
            error_handler("Err - pthread_create");
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(thread[i], NULL) != 0)
                error_handler("Err - pthread_join");
    }
    printf("Fuel: %d\n", fuel);

    pthread_mutex_destroy(&mutexFuel);
    pthread_mutexattr_destroy(&recursiveMutexAttributes);

    return 0;
}
