/**
  *  TODO: program description
  */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_END    0
#define WRITE_END   1

const int MAX_LINE = 80;  //  the maximum command length
const int MAX_COMMANDS = MAX_LINE / 2 + 1;  //  maximum number of commands

void read_command(char* buf) {
    char* input_line = NULL;
    size_t input_size = 0;
    getline(&input_line, &input_size, stdin);
    strcpy(buf, input_line);
}

//  Function that splits a command (stored in buf) into arguments (args)
void tokenize(char* buf, char *args[MAX_COMMANDS]) {
    const char sep[4] = " \n";
    char *token = strtok(buf, sep);

    int cnt = 0;
    while (token != NULL) {
        //  Dynamically allocate memory for each argument.
        args[cnt] = malloc(strlen(token));
        strcpy(args[cnt], token);
        cnt++;
        token = strtok(NULL, sep);
    }

    args[cnt] = NULL;
}

void print_args(char** args) {
    int cnt = 0;

    while (args[cnt] != NULL) {
        printf("%s\n", args[cnt]);
        cnt++;
    }
}

int redirect_operator(char** args) {
    int cnt = 0;

    while (args[cnt] != NULL) {
        if (strcmp(args[cnt], ">") == 0 || strcmp(args[cnt], "<") == 0) {
            return cnt;
        }
        cnt++;
    }

    return -1;
}

int pipe_operator(char** args) {
    int cnt = 0;

    while (args[cnt] != NULL) {
        if (strcmp(args[cnt], "|") == 0) {
            return cnt;
        }
        cnt++;
    }

    return -1;
}

//  Function that deallocates the memory used for storing the command args.
void deallocate_args(char** args) {
    int cnt = 0;

    while (args[cnt] != NULL) {
        free(args[cnt]);
        cnt++;
    }
}

//  This function handles a command which contains a pipe
void run_pipe_command(char** args, int pipe_pos) {
    free(args[pipe_pos]);
    args[pipe_pos] = NULL;

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe() failed\n");
        exit(errno);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Second fork() failed!");
        exit(errno);
    } else if (pid > 0) {  //  child
        close(pipe_fd[READ_END]);
        dup2(pipe_fd[WRITE_END], STDOUT_FILENO);  //  the first command writes to the pipe
        close(pipe_fd[WRITE_END]);

        execvp(args[0], args);
    } else {  //  child of child
        close(pipe_fd[WRITE_END]);
        dup2(pipe_fd[READ_END], STDIN_FILENO);   //  the second command reads from the pipe
        close(pipe_fd[READ_END]);

        execvp(args[pipe_pos + 1], args + (pipe_pos + 1));
    }
}

int main(int argc, char **argv) {
    int should_run = 1;  //  flag to determine when to exit the program
    char* last_command = NULL;
    char* buf = malloc(MAX_LINE);
    char* args[MAX_COMMANDS];  //  command arguments

    while (should_run) {
        printf("myshell>> ");
        read_command(buf);

        //  check if user wants to run last command
        if (strcmp(buf, "!!\n") == 0) {
            if (last_command == NULL) {
                printf("No previous command in history!\n");
                continue;
            } else {
                strcpy(buf, last_command);
                printf("%s", buf);
            }
        }

        //  update the last command
        if (last_command != NULL) {
            free(last_command);
        }
        last_command = malloc(sizeof(buf) + 1);
        strcpy(last_command, buf);

        //  split the command into tokens
        tokenize(buf, args);

        //  create a child process to run the user command in
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork() failed");
            return errno;
        } else if (pid == 0) {  //  child process
            int redirect_pos = redirect_operator(args);
            int pipe_pos = pipe_operator(args);

            if (redirect_pos == -1) {  //  no redirects
                if (pipe_pos == -1) {  //  no pipe operator
                    execvp(args[0], args);
                } else {
                    run_pipe_command(args, pipe_pos);
                    exit(0);
                }
            } else if (strcmp(args[redirect_pos], ">") == 0) {  //  output redirect
                int fd_out = open(args[redirect_pos + 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
                if (fd_out < 0) {
                    perror("Could not open the output redirect file");
                    return errno;
                }

                dup2(fd_out, STDOUT_FILENO);  //  any writes to stdout will in fact be sent to the output file
                close(fd_out);

                free(args[redirect_pos]);
                free(args[redirect_pos + 1]);
                args[redirect_pos] = args[redirect_pos + 1] = NULL;

                execvp(args[0], args);
            } else {  //  input redirect
                int fd_in = open(args[redirect_pos + 1], O_RDONLY, S_IRWXU);
                if (fd_in < 0) {
                    perror("Could not open the input redirect file");
                    return errno;
                }

                dup2(fd_in, STDIN_FILENO);  //  read from the given file instead of stdin
                close(fd_in);

                free(args[redirect_pos]);
                free(args[redirect_pos + 1]);
                args[redirect_pos] = args[redirect_pos + 1] = NULL;

                execvp(args[0], args);
            }
        } else {  //  parent process
            int run_in_background = (buf[strlen(buf) - 1] == '&');
            if (!run_in_background) {
                wait(NULL);
            }

            deallocate_args(args);
        }
    }

    return 0;
}
