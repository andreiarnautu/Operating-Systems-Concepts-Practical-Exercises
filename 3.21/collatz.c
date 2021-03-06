/**
  *  This program generates the Collatz sequence starting with a given number in a child process.
  */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char **argv) {
    int starting_number = atoi(argv[1]);

    if (starting_number < 0) {
        perror("The starting number must not be negative!\n");
        return errno;
    }

    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("fork() failed");
        return errno;
    } else if (pid == 0) {  //  child process
        while (starting_number != 1) {
            printf("%d ", starting_number);

            if (starting_number % 2 == 0) {
                starting_number /= 2;
            } else {
                starting_number = starting_number * 3 + 1;
            }
        }

        printf("%d\n", starting_number);
    } else {  //  parent process
        int child_status;
        wait(&child_status);
        printf("Child process terminated with status %d\n", child_status);
    }
    return 0;
}
