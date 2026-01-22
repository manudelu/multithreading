/*
    Exercise: Multithreaded Dice Roller

    This program creates multiple threads, where each thread simulates 
    rolling a six-sided die by generating a random number from 1 to 6. 
    Each thread returns its result to the main thread using dynamically 
    allocated memory. The main thread waits for all threads to finish, 
    prints each die roll result, and frees the memory returned by each 
    thread.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 8

void* roll_dice(void* arg){
    int value = (rand() % 6) + 1;
    int* result = malloc(sizeof(int));
    *result = value;
    return (void*) result;
    // Intead of return we can call pthread_exit((void*) result);
}

static void error_handler(void *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    int *res;
    srand(time(NULL));
    pthread_t thread[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread[i], NULL, &roll_dice, NULL) != 0)
            error_handler("pthread_create");
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(thread[i], (void**) &res) != 0)
                error_handler("pthread_join");
        printf("Result: %d\n", *res);
        free(res);
    }
           
    return 0;
}