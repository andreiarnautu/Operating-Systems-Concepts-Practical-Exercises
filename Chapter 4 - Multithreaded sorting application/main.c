/**
 *  A program that sorts using multithreading.
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

const int MAX_DEPTH = 4;
const int MIN_PARTITION_SIZE = 128;

void* sort_thread(void* args);

struct SortParams {
    int* array;
    int len;
    int depth;
};

int* generate_array(int n) {
    srand(time(0));

    int* array = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        array[i] = rand() % 100000;  //  for visibility purposes
    }

    return array;
}

//  Comparator for qsort()
int cmp(const void* arg1, const void* arg2) {
    const int* lhs = arg1;
    const int* rhs = arg2;

    if (*lhs < *rhs) {
        return -1;
    } else if (*lhs == *rhs) {
        return 0;
    } else {
        return 1;
    }
}

//  Function that merges to sorted halves of an array
void merge(int* start, int* mid, int* end) {
    int* result = malloc((end - start) * sizeof(int));
    int* lhs = start, *rhs = mid, *dst = result;

    while (lhs != mid && rhs != end) {
        if (*lhs < *rhs) {
            *dst++ = *lhs++;
        } else {
            *dst++ = *rhs++;
        }
    }

    while (lhs != mid) {
        *dst++ = *lhs++;
    }

    //  Copy the result into the original array
    memcpy(start, result, (rhs - start) * sizeof(int));
    free(result);
}

void sort_multithread(int* array, int len, int depth) {
    if (len < 2) return;

    //  If we reached the maximum number of threads (depth = 0) or the partition is small enough,
    //  then just call qsort() to sort the current partition
    if (depth == 0 || len < MIN_PARTITION_SIZE) {
        qsort(array, len, sizeof(int), cmp);
        return;
    }

    struct SortParams params = { array, len / 2, depth - 1 };
    pthread_t t_id;

    //  Create a new thread to sort the first half of the partition
    pthread_create(&t_id, NULL, sort_thread, &params);

    //  Use the current thread to sort the second half of the partition.
    //  No need to create a new thread for this, since we can just use the current one.
    sort_multithread(array + len / 2, len - len / 2, depth - 1);

    //  Join the created thread
    pthread_join(t_id, NULL);

    //  Merge the sorted partitions
    merge(array, array + len / 2, array + len);
}

//  This is the function that gets called upon creating a new thread.
//  It just passes the parameters to our sorting algorithm
void* sort_thread(void* args) {
    struct SortParams* params = args;
    sort_multithread(params->array, params->len, params->depth);
    return args;
}

//  Public sorting API
void sort(int* array, int len) {
    //  MAX_DEPTH = n will use (1 << n) - 1 threads.
    sort_multithread(array, len, MAX_DEPTH);
}

void print_sorted_array(int* array, int len) {
    //  This is here just as a sanity check. Lol.
    for (int i = 1; i < len; i++) {
        if (array[i] < array[i - 1]) {
            printf("The array is not sorted!...:(\n");
            return;
        }
    }
    
    //  Let's print the sorted array in a nice way.
    printf("The sorted array is:\n");
    for (int i = 0; i < len; i++) {
        printf("%5d ", array[i]);

        if (i % 10 == 9) {
            printf("\n");
        }
    }
    printf("\n");
}

int main(int argc, char** argv) {
    const int n = atoi(argv[1]);
    int* array = generate_array(n);
    sort(array, n);
    print_sorted_array(array, n);
    return 0;
}
