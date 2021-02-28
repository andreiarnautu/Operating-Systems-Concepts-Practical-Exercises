#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

const int MAX_FILE_NAME_LENGTH = 50;

int main() {
    char *input_file_name = malloc(MAX_FILE_NAME_LENGTH * sizeof(char));
    char *output_file_name = malloc(MAX_FILE_NAME_LENGTH * sizeof(char));

    printf("Give input file name:\n");
    scanf("%s", input_file_name);
    printf("Give output file name:\n");
    scanf("%s", output_file_name);

    int input_fd = open(input_file_name, O_RDONLY);
    int output_fd = open(output_file_name, O_CREAT | O_RDWR, S_IRWXU);

    if (input_fd < 0) {
        printf("Error opening the input file.\n");
        return 0;
    }
    if (output_fd < 0) {
        printf("Error opening the output file.\n");
        return 0;
    }
    
    //  Obtain the size of the input file
    struct stat input_statbuf;
    stat(input_file_name, &input_statbuf);

    char *buf = malloc(input_statbuf.st_size);
    int nread = read(input_fd, buf, input_statbuf.st_size);
    
    if (nread < 0) {
        perror("Reading error");
        return errno;
    }

    int nwrite = write(output_fd, buf, nread);
    if (nwrite < 0) {
        perror("Writing error");
        return errno;
    }

    printf("Contents of the input file successfully copied to the output file!");

    close(input_fd);
    close(output_fd);
    return 0;
}
