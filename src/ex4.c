/*
    Exercise: Producer-Consumer with Cars and Fuel

    This program simulates a fuel station with cars (consumers) and fuel
    suppliers (producers). Multiple car threads attempt to consume 40 units
    of fuel each. If there is not enough fuel, cars wait on a condition
    variable. Fuel-filling threads periodically add 30 units of fuel and
    broadcast a signal to wake waiting cars. A mutex ensures mutual
    exclusion when accessing the shared fuel variable, preventing data
    races. The program demonstrates thread synchronization using mutexes
    and condition variables.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define NUM_THREADS 6

pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;
int fuel = 0;

void* car(void* arg) {
    pthread_mutex_lock(&mutexFuel);
    while (fuel < 40) {
        printf("No fuel. Waiting...\n");
        pthread_cond_wait(&condFuel, &mutexFuel);
        // Equivalent to: 
        // pthread_mutex_unlock(&mutexFuel);
        // Wait for signal on condFuel
        // pthread_mutex_lock(&mutexFuel);
    }
    fuel -= 40;
    printf("Got fuel. Now left: %d\n", fuel);
    pthread_mutex_unlock(&mutexFuel);
    return NULL;
}

void* fuel_filling(void* arg) {

    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&mutexFuel);
        fuel += 30;
        printf("Filled fuel... %d\n", fuel);
        pthread_mutex_unlock(&mutexFuel);
        pthread_cond_broadcast(&condFuel);
        sleep(1);
    }    
    return NULL;
}

static void error_handler(void * error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    pthread_t thread[NUM_THREADS];

    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&condFuel, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (i == 4 || i == 5) {
            if (pthread_create(&thread[i], NULL, &fuel_filling, NULL) != 0)
                error_handler("pthread_create");
        }
        else {
            if (pthread_create(&thread[i], NULL, &car, NULL) != 0)
                error_handler("pthread_create"); 
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(thread[i], NULL) != 0)
            error_handler("pthread_join");
    }
    
    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&condFuel);

    return 0;
}