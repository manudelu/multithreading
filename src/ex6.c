/*
    Exercise: Dice Game Using Thread Barriers

    This program simulates a continuous dice game played by multiple
    threads. Each thread rolls a die and waits at a barrier until all
    other threads (and the main thread) have finished rolling. The main
    thread then determines the highest roll, marks the winners, and waits
    at a second barrier to release the threads for the next round.
    
    Two pthread barriers are used:
        - barrierRollDice: synchronizes all dice rolls each round.
        - barrierCalculated: ensures threads wait until the main thread
          determines the winner.
    
    This exercise demonstrates multi-thread synchronization using
    pthread_barrier_t and shared memory for coordinating repeated rounds
    of computation.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 8

int diceValues[NUM_THREADS];
int status[NUM_THREADS] = {0};
pthread_barrier_t barrierRollDice;
pthread_barrier_t barrierCalculated;

void* roll_dice(void* arg){
    int index = *(int*)arg;
    while (1) {
        diceValues[index] = rand() % 6 + 1;
        pthread_barrier_wait(&barrierRollDice);
        pthread_barrier_wait(&barrierCalculated);
        if (status[index] == 1) {
            printf("(%d rolled %d) I won\n", index, diceValues[index]);
        }
        else {
            printf("(%d rolled %d) I lost\n", index, diceValues[index]);
        }
    }
    free(arg);
    return NULL;
}

static void error_handler(void *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    pthread_t thread[NUM_THREADS];
    pthread_barrier_init(&barrierRollDice, NULL, NUM_THREADS+1);
    pthread_barrier_init(&barrierCalculated, NULL, NUM_THREADS+1);

    for (int i = 0; i < NUM_THREADS; i++) {
        int* a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&thread[i], NULL, &roll_dice, (void*) a) != 0)
            error_handler("pthread_create");
    }

    while(1) {
        pthread_barrier_wait(&barrierRollDice);
        // Calculate the winner
        int max = 0;
        for (int i = 0; i < NUM_THREADS; i++) {
            if (diceValues[i] > max) {
                max = diceValues[i];
            }
        }
        for (int i = 0; i < NUM_THREADS; i++) {
            if (diceValues[i] == max) {
                status[i] = 1;
            } else {
                status[i] = 0;
            }
        }
        sleep(1);
        printf("==== New round starting ====\n");
        pthread_barrier_wait(&barrierCalculated);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(thread[i], NULL) != 0)
            error_handler("pthread_join");
    }
    
    pthread_barrier_destroy(&barrierRollDice);
    pthread_barrier_destroy(&barrierCalculated);

    return 0;
}