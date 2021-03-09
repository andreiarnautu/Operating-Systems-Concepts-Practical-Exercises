/**
  *  This is a file-copying program:
  *     - it receives the name of the file to be copied and the name of the destination file
  *     - the program creates an ordinary pipe and writes the contents of the original file to the pipe
  *     - the child process reads the file from the pipe and writes to the destination file
  */
#include <fcntl.h>  //  permission flags
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_END    0
#define WRITE_END   1
#define BUFFER_SIZE 25

int main(int argc, char **argv) {
    //  open source and destination files
    int input_fd = open(argv[1], O_RDONLY);
    int output_fd = open(argv[2], O_CREAT | O_WRONLY, S_IRWXU);

    //  check for potential errors
    if (input_fd < 0) {
        perror("Error opening the source file\n");
        return errno;
    }
    if (output_fd < 0) {
        perror("Error opening the destination file\n");
        return errno;
    }

    //  open pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe() failed\n");
        return errno;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork() failed\n");
        return errno;
    } else if (pid == 0) {  //  child process
        char *buf = malloc(BUFFER_SIZE);
        int nread = read(pipe_fd[READ_END], buf, BUFFER_SIZE);
        if (nread < 0) {
            perror("Error reading from the pipe\n");
            return errno;
        }

        //  close the end of the pipe unused by this process
        close(pipe_fd[WRITE_END]);
        while (nread > 0) {
            int nwrite = write(output_fd, buf, nread);
            if (nwrite < 0) {
                perror("Error writing to the destination file\n");
                return errno;
            }

            nread = read(pipe_fd[READ_END], buf, BUFFER_SIZE);
            if (nread < 0) {
                perror("Error reading from the pipe\n");
                return errno;
            }
        }

        close(output_fd);
    } else {  //  parent process
        char *buf = malloc(BUFFER_SIZE);
        int nread = read(input_fd, buf, BUFFER_SIZE);
        if (nread < 0) {
            perror("Error reading from the source file\n");
            return errno;
        }

        //  close the end of the pipe unused by this process
        close(pipe_fd[READ_END]);
        while (nread > 0) {
            int nwrite = write(pipe_fd[WRITE_END], buf, nread);
            if (nwrite < 0) {
                perror("Error writing to the pipe\n");
                return errno;
            }

            nread = read(input_fd, buf, BUFFER_SIZE);
            if (nread < 0) {
                perror("Error reading from the source file\n");
                return errno;
            }
        }

        close(input_fd);
    }
    return 0;
}
