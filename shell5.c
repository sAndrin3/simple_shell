#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define ARGUMENTS_SIZE 64

char *get_command_path(char *command, char *path)
{
    char *dir;
    char *token;
    char *path_copy = strdup(path);

    token = strtok(path_copy, ":");

    while (token != NULL) {
        dir = malloc(strlen(token) + strlen(command) + 2);
        strcpy(dir, token);
        strcat(dir, "/");
        strcat(dir, command);

        if (access(dir, X_OK) == 0) {
            free(path_copy);
            return dir;
        }

        free(dir);
        token = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

void print_environment(void)
{
    extern char **environ;

    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
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
            
            char *command = arguments[0];
            
            if (strcmp(command, "exit") == 0) {
                printf("Exiting the shell...\n");
                break;
            } else if (strcmp(command, "env") == 0) {
                print_environment();
                continue;
            }
            
            char *path = getenv("PATH");
            char *command_path = get_command_path(command, path);

            if (command_path != NULL) {
                pid_t pid = fork();

                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Child process
                    if (execve(command_path, arguments, NULL) == -1) {
                        perror("execve");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    // Parent process
                    wait(NULL);
                }

                free(command_path);
            } else {
                printf("%s: command not found\n", command);
            }
        }
    }

    return 0;
}

