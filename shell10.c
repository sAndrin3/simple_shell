#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

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
    } else if (strcmp(command, "cd") == 0) {
        if (arguments[1] == NULL || strcmp(arguments[1], "~") == 0) {
            char *home_dir = getenv("HOME");
            if (home_dir == NULL) {
                fprintf(stderr, "cd: HOME environment variable not set\n");
                return -1;
            }
            if (chdir(home_dir) == -1) {
                perror("chdir");
                return -1;
            }
        } else if (strcmp(arguments[1], "-") == 0) {
            char *previous_dir = getenv("OLDPWD");
            if (previous_dir == NULL) {
                fprintf(stderr, "cd: OLDPWD environment variable not set\n");
                return -1;
            }
            if (chdir(previous_dir) == -1) {
                perror("chdir");
                return -1;
            }
        } else {
            char current_dir[PATH_MAX];
            if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
                perror("getcwd");
                return -1;
            }
            if (chdir(arguments[1]) == -1) {
                perror("chdir");
                return -1;
            }
            setenv("OLDPWD", current_dir, 1);
        }
        char *current_dir = getcwd(NULL, 0);
        if (current_dir == NULL) {
            perror("getcwd");
            return -1;
        }
        setenv("PWD

