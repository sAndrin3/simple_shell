#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
    char *prompt = "#cisfun$ ";
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    while (1) {
        printf("%s", prompt);
        bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);

        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        } else if (bytes_read == 0) {
            printf("\n");
            break;
        } else {
            buffer[bytes_read - 1] = '\0'; // Remove the trailing newline character

            if (access(buffer, X_OK) == 0) {
                pid_t pid = fork();

                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Child process
                    if (execve(buffer, NULL, NULL) == -1) {
                        perror("execve");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    // Parent process
                    wait(NULL);
                }
            } else {
                printf("%s: No such file or directory\n", buffer);
            }
        }
    }

    return 0;
}

