#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define ARGUMENTS_SIZE 64

char **parse_arguments(char *line)
{
    static char *arguments[ARGUMENTS_SIZE];
    int argument_count = 0;

    char *token = line;
    while (*line != '\0') {
        if (*line == ' ') {
            *line = '\0'; // Replace space with null terminator
            arguments[argument_count] = token;
            argument_count++;
            token = line + 1;
        }
        line++;
    }

    arguments[argument_count] = token;
    argument_count++;
    arguments[argument_count] = NULL; // Set the last element to NULL for execve

    return arguments;
}

int main(void)
{
    char *prompt = "$ ";
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

            char **arguments = parse_arguments(buffer);
            char *command = arguments[0];

            if (strcmp(command, "exit") == 0) {
                printf("Exiting the shell...\n");
                break;
            }

            pid_t pid = fork();

            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                // Child process
                if (execvp(command, arguments) == -1) {
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
            } else {
                // Parent process
                wait(NULL);
            }
        }
    }

    return 0;
}

