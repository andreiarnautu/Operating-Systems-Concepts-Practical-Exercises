/**
 *  A program that manages a finite number of resources which are requested by multiple consumers.
 *  The number of resources requested by any consumer is not greater that the total number of resources.
 */
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

const int MAX_RESOURCES = 5;

pthread_mutex_t mtx;
int available_resources = MAX_RESOURCES;

void decrease_count(int count) {
    int flag = 0;
    while (!flag) {
        pthread_mutex_lock(&mtx);
        if (available_resources >= count) {
            available_resources -= count;
            printf("Gave %d resources, %d remaining.\n", count, available_resources);
            flag = 1;
        }
        pthread_mutex_unlock(&mtx);
    }
}

void increase_count(int count) {
    pthread_mutex_lock(&mtx);
    available_resources += count;
    printf("Received %d resources, %d remaining.\n", count, available_resources);
    pthread_mutex_unlock(&mtx);
}

void* get_resources(void* args) {
    int count = ((int*)args)[0];

    decrease_count(count);
    sleep(0.02);
    increase_count(count);

    return NULL;
}

int main(int argc, char** argv) {
    if (pthread_mutex_init(&mtx, NULL)) {
        perror(NULL);
        return errno;
    }

    //  Create a separate thread for each request.
    //  Each thread will receive the number of requested resources as a parameter.
    pthread_t tid[argc - 1];
    int* count[argc - 1];
    for (int i = 0; i < argc - 1; i++) {
        count[i] = (int*) malloc(sizeof(int));
        *count[i] = atoi(argv[i + 1]);
    }

    for (int i = 0; i < argc - 1; i++) {
        if (pthread_create(&tid[i], NULL, get_resources, count[i])) {
            perror(NULL);
            return errno;
        }
    }

    for (int i = 0; i < argc - 1; i++) {
        if (pthread_join(tid[i], NULL)) {
            perror(NULL);
            return errno;
        }
    }

    for (int i = 0; i < argc - 1; i++) {
        free(count[i]);
    }
    pthread_mutex_destroy(&mtx);
    return 0;
}
