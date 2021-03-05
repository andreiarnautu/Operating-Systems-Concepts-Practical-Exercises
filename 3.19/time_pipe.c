/**
  *  This program measures the amount of time necessary to run a command from command line (e.g. ls, pwd).
  *  It runs the command in a child process.
  *  Before executing the given command, the child process obtains the starting timestamp and passes it to the parent (main) process using a pipe.
  *  After the child finishes executing, the parent process obtains the final timestamp and computes the time difference.
  */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

const int READ_END = 0;
const int WRITE_END = 1;
const int MAX_COMMAND_SIZE = 100;

int main(int argc, char **argv) {
    //  open pipe
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe() failed");
        return errno;
    }

    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("fork() failed");
        return errno;
    } else if (pid == 0) {  //  child process
        //  close the unused end of the pipe
        close(fd[READ_END]);

        char* command = (char*) malloc(MAX_COMMAND_SIZE);
        strcat(command, "/bin/");
        strcat(command, argv[1]);

        //  get starting UNIX time
        struct timeval current_time;
        gettimeofday(&current_time, NULL);

        //  send the current time using the pipe
        struct timeval *write_msg = malloc(sizeof(struct timeval));
        write_msg[0] = current_time;
        write(fd[WRITE_END], write_msg, sizeof(struct timeval));

        //  execute the given command
        execlp(command, argv[1], NULL);

        //  close the write end of the pipe
        close(fd[WRITE_END]);
    } else {  //  parent process
        //  close the unused end of the pipe
        close(fd[WRITE_END]);

        int child_status;
        wait(&child_status);

        //  get finish UNIX time
        struct timeval finish_time;
        gettimeofday(&finish_time, NULL);

        printf("Child terminated with status %d\n", child_status);

        struct timeval *read_msg = malloc(sizeof(struct timeval));
        read(fd[READ_END], read_msg, sizeof(struct timeval));

        //  compute time elapsed
        long long elapsed_time = finish_time.tv_sec * 1e6 + finish_time.tv_usec -
                                    read_msg[0].tv_sec * 1e6 - read_msg[0].tv_usec;

        double elapsed_seconds = elapsed_time / (double)1e6;
        printf("The command took %.6f seconds to execute\n", elapsed_seconds);

        close(fd[READ_END]);
    }
    return 0;
}
