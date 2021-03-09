/**
  *  This program creates 2 processes:
  *     - the first one sends a message to the second one
  *     - the second one reverses the case of the letters in the message and sends the new message back
  *     - the communication between processes is achieved using ordinary pipes
  */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

const char* INITIAL_MESSAGE = "This iS a meSSaGe";
const int BUFFER_SIZE = 32;

void parent_process(int pipe_fd_initial[2], int pipe_fd_final[2]) {
    //  close the unused ends of the initial and final pipes
    close(pipe_fd_initial[READ_END]);
    close(pipe_fd_final[WRITE_END]);

    //  send initial message
    write(pipe_fd_initial[WRITE_END], INITIAL_MESSAGE, strlen(INITIAL_MESSAGE) + 1);
    close(pipe_fd_initial[WRITE_END]);

    //  Wait for the child to terminate
    int child_status;
    wait(&child_status);
    printf("Child terminated with status %d\n", child_status);

    char* altered_message = malloc(BUFFER_SIZE);
    read(pipe_fd_final[READ_END], altered_message, BUFFER_SIZE);
    close(pipe_fd_final[READ_END]);

    printf("altered message:\n%s\n", altered_message);
}

void child_process(int pipe_fd_initial[2], int pipe_fd_final[2]) {
    //  close the unused ends of the initial and final pipes
    close(pipe_fd_initial[WRITE_END]);
    close(pipe_fd_final[READ_END]);

    //  read the initial message
    char* message = malloc(BUFFER_SIZE);
    read(pipe_fd_initial[READ_END], message, BUFFER_SIZE);
    close(pipe_fd_initial[READ_END]);

    //  perform string transformations
    for (int i = 0; i < (int)strlen(message); i++) {
        if ('a' <= message[i] && message[i] <= 'z') {
            message[i] += ('A' - 'a');
        } else if ('A' <= message[i] && message[i] <= 'Z') {
            message[i] += ('a' - 'A');
        }
    }

    //  send altered message
    write(pipe_fd_final[WRITE_END], message, BUFFER_SIZE);
    close(pipe_fd_final[WRITE_END]);
    exit(0);
}

int main(int argc, char **argv) {
    //  open 2 pipes - (parent -> child) and (child -> parent)
    int pipe_fd_initial[2], pipe_fd_final[2];

    if (pipe(pipe_fd_initial) == -1) {
        perror("#1 pipe() failed");
        return errno;
    }

    if (pipe(pipe_fd_final) == -1) {
        perror("#2 pipe() failed");
        return errno;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork() failed");
        return errno;
    } else if (pid == 0) {
        child_process(pipe_fd_initial, pipe_fd_final);
    } else {
        parent_process(pipe_fd_initial, pipe_fd_final);
    }


    return 0;
}
