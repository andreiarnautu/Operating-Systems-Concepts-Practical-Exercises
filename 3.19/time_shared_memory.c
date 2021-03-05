/**
  *  This program measures the amount of time necessary to run a command from command line (e.g. ls, pwd).
  *  It runs the command in a child process.
  *  Before executing the given command, the child process obtains the starting timestamp and passes it to the parent (main) process using shared memory.
  *  After the child finishes executing, the parent process obtains the final timestamp and computes the time difference.
  */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

const int MAX_COMMAND_SIZE = 100;
const int SHARED_MEMORY_SIZE = 4096;

int main(int argc, char **argv) {
    pid_t pid;

    pid = fork();


    const char *name = "TIME";
    //  create the shared memory object
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror(NULL);
        return errno;
    }
    //  configure the size of the memory object
    if (ftruncate(fd, SHARED_MEMORY_SIZE) == -1) {
        perror(NULL);
        shm_unlink(name);
        return errno;
    }

    if (pid < 0) {
        printf("fork() failed!");
        return 1;
    } else if (pid == 0) {  //  child process
        char* command = (char*) malloc(MAX_COMMAND_SIZE);
        strcat(command, "/bin/");
        strcat(command, argv[1]);

        //  memory map the shared memory object
        struct timeval *ptr = (struct timeval*) mmap(0, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            perror(NULL);
            return errno;
        }
        //  get starting UNIX time
        struct timeval current_time;
        gettimeofday(&current_time, NULL);

        //  save starting time in shared memory
        ptr[0] = current_time;
        execlp(command, argv[1], NULL);
        exit(0);
    } else {  //  parent process
        int child_status;
        wait(&child_status);

        //  get finish UNIX time
        struct timeval finish_time;
        gettimeofday(&finish_time, NULL);

        printf("Child terminated with status %d\n", child_status);

        struct timeval *ptr = (struct timeval*) mmap(0, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            shm_unlink(name);
            perror(NULL);
            return errno;
        }

        struct timeval start_time = ptr[0];
        long long elapsed_time = finish_time.tv_sec * 1e6 + finish_time.tv_usec -
                                 start_time.tv_sec * 1e6 - start_time.tv_usec;

        double elapsed_seconds = elapsed_time / (double)1e6;
        printf("The command took %.6f seconds to execute\n", elapsed_seconds);
    }

    shm_unlink(name);
    return 0;
}
