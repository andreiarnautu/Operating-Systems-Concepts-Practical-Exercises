/**
 *  Program that receives a series of positive numbers from command line and calculates:
 *      - the average of the numbers
 *      - the maximum value
 *      - the minimum value
 *  For each of the 3 values, we'll use a different thread.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

double average = 0;
int minimum = -1;
int maximum = -1;

void f_average(void *args) {
    int argc = ((int*)args)[0];

    for (int i = 1; i <= argc; i++) {
        average += ((int*)args)[i];
    }

    average /= (double)argc;
}

void f_minimum(void *args) {
    int argc = ((int*)args)[0];

    for (int i = 1; i <= argc; i++) {
        if (minimum == -1 || ((int*)args)[i] < minimum) {
            minimum = ((int*)args)[i];
        }
    }
}

void f_maximum(void *args) {
    int argc = ((int*)args)[0];

    for (int i = 1; i <= argc; i++) {
        if (((int*)args)[i] > maximum) {
            maximum = ((int*)args)[i];
        }
    }
}

int main(int argc, char **argv) {
    pthread_t t_id[3];

    //  Create the array of parameters that will be passed to each thread
    int* numbers = (int*) malloc(sizeof(int) * argc);
    numbers[0] = argc - 1;
    for (int i = 1; i < argc; i++) {
        numbers[i] = atoi(argv[i]);
    }

    //  Create threads
    if (pthread_create(&t_id[0], NULL, f_average, numbers)) {
        printf("Error creating thread 0\n");
        return 1;
    }

    if (pthread_create(&t_id[1], NULL, f_minimum, numbers)) {
        printf("Error creating thread 1\n");
        return 1;
    }

    if (pthread_create(&t_id[2], NULL, f_maximum, numbers)) {
        printf("Error creating thread 2\n");
        return 1;
    }

    //  Join threads
    for (int i = 0; i < 3; i++) {
        if (pthread_join(t_id[i], NULL)) {
            printf("Error joining thread %d\n", i);
            return 1;
        }
    }

    printf("Average: %.2f\n", average);
    printf("Minimum: %d\n", minimum);
    printf("Maximum: %d\n", maximum);
    return 0;
}
