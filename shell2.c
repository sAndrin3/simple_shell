#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define ARGUMENTS_SIZE 64

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
            
            char *arguments[ARGUMENTS_SIZE];
            char *token;
            int argument_count = 0;

            token = strtok(buffer, " ");

            while (token != NULL && argument_count < ARGUMENTS_SIZE - 1) {
                arguments[argument_count] = token;
                argument_count++;
                token = strtok(NULL, " ");
            }
            
            arguments[argument_count] = NULL; // Set the last element to NULL for execve

            if (access(arguments[0], X_OK) == 0) {
                pid_t pid = fork();

                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Child process
                    if (execve(arguments[0], arguments, NULL) == -1) {
                        perror("execve");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    // Parent process
                    wait(NULL);
                }
            } else {
                printf("%s: command not found\n", arguments[0]);
            }
        }
    }

    return 0;
}

