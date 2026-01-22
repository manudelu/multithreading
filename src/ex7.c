/*
    Exercise: Demonstrating Deadlock with Multiple Mutexes

    This program simulates threads updating shared resources (fuel and
    water) protected by two mutexes. Each thread randomly decides the
    order in which it locks the mutexes. 

        - If all threads lock mutexes in the same order, there is no deadlock.
        - If threads lock mutexes in different orders, a deadlock may occur.

    The exercise demonstrates the risk of deadlocks in multithreaded
    programs when multiple locks are acquired in inconsistent order and
    highlights the importance of lock ordering to prevent deadlocks.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 8

pthread_mutex_t mutexFuel;
pthread_mutex_t mutexWater;
int fuel = 50;
int water = 10;

void* routine(void* arg){
    // NO DEADLOCK
    if (rand()%2 == 0) {
        pthread_mutex_lock(&mutexFuel);
        sleep(1);
        pthread_mutex_lock(&mutexWater);
    // DEADLOCK
    } else {
        pthread_mutex_lock(&mutexWater);
        sleep(1);
        pthread_mutex_lock(&mutexFuel);
    }    
    fuel += 50;
    water = fuel;
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexWater);
    printf("Incremented fuel to %d and set water to %d\n", fuel, water);
    return NULL;
}

static void error_handler(void *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    pthread_t thread[NUM_THREADS];

    pthread_mutex_init(&mutexFuel, NULL);
    pthread_mutex_init(&mutexWater, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread[i], NULL, &routine, NULL) != 0)
            error_handler("Err - pthread_create");
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(thread[i], NULL) != 0)
                error_handler("Err - pthread_join");
    }
    printf("Fuel: %d\n", fuel);
    printf("Water: %d\n", water);
    pthread_mutex_destroy(&mutexFuel);
    pthread_mutex_destroy(&mutexWater);

    return 0;
}
