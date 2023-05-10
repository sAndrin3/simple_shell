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
    arguments[argument_count] = NULL; // Set the last element to NULL for execvp

    return arguments;
}

int execute_command(char **arguments)
{
    char *command = arguments[0];

    if (strcmp(command, "exit") == 0) {
        if (arguments[1] != NULL) {
            int status = atoi(arguments[1]);
            printf("Exiting the shell with status %d...\n", status);
            exit(status);
        } else {
            printf("Exiting the shell...\n");
            exit(EXIT_SUCCESS);
        }
    } else if (strcmp(command, "setenv") == 0) {
        if (arguments[1] == NULL || arguments[2] == NULL) {
            fprintf(stderr, "setenv: Missing arguments\n");
            return -1;
        }
        if (setenv(arguments[1], arguments[2], 1) == -1) {
            perror("setenv");
            return -1;
        }
    } else if (strcmp(command, "unsetenv") == 0) {
        if (arguments[1] == NULL) {
            fprintf(stderr, "unsetenv: Missing argument\n");
            return -1;
        }
        if (unsetenv(arguments[1]) == -1) {
            perror("unsetenv");
            return -1;
        }
    } else {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            return -1;
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

    return 0;
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
            int status = execute_command(arguments);

            if (status == -1) {
                fprintf(stderr, "Command execution failed\n");
            }
        }
    }

    return 0;
}

