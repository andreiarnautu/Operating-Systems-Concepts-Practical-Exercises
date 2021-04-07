/**
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int THREAD_COUNT = 27;

int is_valid = 1;

int mat[9][9];

void check_square(void* args) {
    int x_start = ((int*)args)[0];
    int y_start = ((int*)args)[1];

    int* seen = calloc(9, sizeof(int));
    for (int i = x_start; i < x_start + 3; i++) {
        for (int j = y_start; j < y_start + 3; j++) {
            if (seen[mat[i][j] - 1]) {
                is_valid = 0;
                free(seen);
                return;
            }
            seen[mat[i][j] - 1] = 1;
        }
    }

    free(seen);
    free(args);
}

void check_line(void* args) {
    int i = ((int*)args)[0];

    int* seen = calloc(9, sizeof(int));
    for (int j = 0; j < 9; j++) {
        if (seen[mat[i][j] - 1]) {
            is_valid = 0;
            free(seen);
            return;
        }
        seen[mat[i][j] - 1] = 1;
    }

    free(seen);
    free(args);
}

void check_column(void* args) {
    int j = ((int*)args)[0];

    int* seen = calloc(9, sizeof(int));
    for (int i = 0; i < 9; i++) {
        if (seen[mat[i][j] - 1]) {
            is_valid = 0;
            free(seen);
            return;
        }
        seen[mat[i][j] - 1] = 1;
    }

    free(seen);
    free(args);
}

int main(int argc, char** argv) {
    //  read matrix
    FILE* fp = fopen(argv[1], "r");

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            fscanf(fp, "%d", &mat[i][j]);
        }
    }

    fclose(fp);

    pthread_t t_id[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; i++) {
        //  9 proc-threads for the lines, 9 proc-threads for the columns and 9 proc-threads for the 3x3 squares
        if (i < 9) {
            int* args = malloc(sizeof(int));
            args[0] = i;

            if (pthread_create(&t_id[i], NULL, check_line, args)) {
                printf("Error creating thread number %d\n", i);
                free(args);
                return 1;
            }
        } else if (i < 18) {
            int* args = malloc(sizeof(int));
            args[0] = i - 9;

            if (pthread_create(&t_id[i], NULL, check_column, args)) {
                printf("Error creating thread number %d\n", i);
                free(args);
                return 1;
            }
        } else {
            int* args = (int*)malloc(2 * sizeof(int));
            args[0] = 3 * ((i - 18) / 3);
            args[1] = 3 * ((i - 18) % 3);

            if (pthread_create(&t_id[i], NULL, check_square, args)) {
                printf("Error creating thread number %d\n", i);
                free(args);
                return 1;
            }
        }
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        if (pthread_join(t_id[i], NULL)) {
            printf("Error joining thread number %d\n", i);
            return 1;
        }
    }

    if (is_valid) {
        printf("Valid solution!\n");
    } else {
        printf("Wrong solution!\n");
    }

    return 0;
}
