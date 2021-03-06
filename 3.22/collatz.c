/**
  *  This program computes the Collatz sequence starting from a given number.
  *  The sequence is computed in a child process which saves the sequence values in a shared-memory object, which are then printed in the parent process.
  */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>  //  O_CREAT, O_RDWR
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>  //  shm_open
#include <sys/mman.h>  //  mmap

#define MAX_SEQUENCE_LENGTH 100

int main(int argc, char **argv) {
    int number = atoi(argv[1]);

    if (number < 0) {
        perror("The starting number must not be negative!\n");
        return errno;
    }

    const char *shm_name = "myshm";
    //  create the shared memory object
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("Failed to create a shared memory object!\n");
        return errno;
    }

    //  configure the size of the memory object
    if (ftruncate(shm_fd, MAX_SEQUENCE_LENGTH * sizeof(int)) == -1) {
        perror("Failed to configure the size of the memory object!\n");
        shm_unlink(shm_name);
        return errno;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork() failed\n");
        return errno;
    } else if (pid == 0) {  //  child process
        //  memory map the shared-memory object in the child process's space
        int *ptr = (int*) mmap(0, MAX_SEQUENCE_LENGTH * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        if (ptr == MAP_FAILED) {
            perror("Failed to map the memory-shared object into child process's space");
            return errno;
        }

        while (number != 1) {
            *ptr = number;
            ptr++;

            if (number % 2 == 0) {
                number /= 2;
            } else {
                number = number * 3 + 1;
            }
        }

        *ptr = number;
        exit(0);
    } else {  //  parent process
        int child_status;
        wait(&child_status);
        printf("Child process terminated with status %d\n", child_status);

        //  memory map the shared-memory object in the parent process's space
        int *ptr = (int*) mmap(0, MAX_SEQUENCE_LENGTH * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        while (*ptr != 0) {
            printf("%d ", *ptr);
            ptr++;
        }
        printf("\n");

        shm_unlink(shm_name);
    }
    return 0;
}
