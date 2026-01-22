/*
    Exercise: Multithreaded Array Summation

    This program splits an array of prime numbers into two equal halves
    and assigns each half to a separate thread. Each thread computes the
    sum of its portion of the array and returns the result to the main
    thread using dynamically allocated memory. The main thread then
    collects both partial sums, frees the returned memory, and computes
    the final global sum.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 2
#define SIZE 10

int primes[SIZE] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29}; 

void* routine(void* arg) {
    int index = *(int*)arg;
    int sum = 0;

    for (int i = 0; i < SIZE/2; i++)
        sum += primes[index + i];

    printf("Local Sum: %d\n", sum);
    *(int*)arg = sum;
    return arg;
}

static void error_handler(void *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    pthread_t thread[NUM_THREADS];
    
    for (int i = 0; i < NUM_THREADS; i++) {
        int* a = malloc(sizeof(int));
        *a = i * (SIZE/2);
        if (pthread_create(&thread[i], NULL, &routine, a) != 0)
            error_handler("pthread_create");
    }

    int globalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        int* r;
        if (pthread_join(thread[i], (void**) &r) != 0)
            error_handler("pthread_join");
        globalSum += *r;
        free(r);
    }
    printf("Global Sum: %d\n", globalSum);

    return 0;
}