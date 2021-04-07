/**
 *  Program using the Monte Carlo technique to approximate the value of pi.
 */
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const double eps = 1e-7;

int points_inside, points_total;

void generate_points(void *args) {
    int count = atoi(((char**)args)[1]);

    srand(time(0));
    for (int i = 0; i < count; i++) {
        double x = (double)rand() / (double)RAND_MAX;
        double y = (double)rand() / (double)RAND_MAX;
        double dist = sqrt(x * x + y * y);

        points_total += 1;
        if (dist <= 1) {
            points_inside++;
        }
    }
}

int main(int argc, char **argv) {

    pthread_t t_id;

    if (pthread_create(&t_id, NULL, generate_points, argv)) {
        printf("Error creating thread!\n");
        return 1;
    }

    if (pthread_join(t_id, NULL)) {
        printf("Error joining thread!\n");
        return 1;
    }

    double pi_approximation = 4 * (double)points_inside / (double)points_total;
    printf("%.10f\n", pi_approximation);
    return 0;
}
