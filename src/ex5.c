/*
    Exercise: Multi-threaded Stove Simulation

    This program simulates multiple chefs (threads) trying to use a limited
    number of stoves (shared resources protected by mutexes). Each stove
    has a fuel amount, and a chef randomly consumes some fuel when using
    a stove. If the stove does not have enough fuel, the chef leaves. If
    all stoves are busy, the chef waits briefly and retries. Mutexes ensure
    that only one chef can access a stove at a time, preventing data races.
    This demonstrates the use of multiple mutexes, try-locks, and thread
    synchronization in a concurrent environment.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 10

// chefs = threads
// stove = shared data (+mutex)
#define SIZE 4
pthread_mutex_t stoveMutex[SIZE];
int stoveFuel[SIZE] = {100, 100, 100, 100};

void* routine(void* arg) {
    for (int i = 0; i < SIZE; i++) {
        if (pthread_mutex_trylock(&stoveMutex[i]) == 0) {
            int fuelNeeded = (rand() % 30);
            if (stoveFuel[i] - fuelNeeded < 0) {
                printf("No more fuel... Going home.\n");
            } else {
                stoveFuel[i] -= fuelNeeded;
                usleep(500000);
                printf("Fuel left: %d\n", stoveFuel[i]);
            }
            pthread_mutex_unlock(&stoveMutex[i]);
            break;
        } else {
            if (i == SIZE-1) {
                printf("No stove available yet. Waiting...\n");
                usleep(300000);
                i = 0;
            }
        }
    }
    return NULL;
}

static void error_handler(const char *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    pthread_t thread[NUM_THREADS];

    for (int i = 0; i < SIZE; i++)
        pthread_mutex_init(&stoveMutex[i], NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread[i], NULL, &routine, NULL) != 0)
            error_handler("pthread_create");
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(thread[i], NULL) != 0)
            error_handler("pthread_join");
    }

    for (int i = 0; i < SIZE; i++)
        pthread_mutex_destroy(&stoveMutex[i]);

    return 0;
}

