#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

#define BUFFER_SIZE 1024
#define ARGUMENTS_SIZE 64

typedef struct Alias {
    char *name;
    char *value;
    struct Alias *next;
} Alias;

Alias *aliases = NULL;

Alias *find_alias(char *name)
{
    Alias *current_alias = aliases;
    while (current_alias != NULL) {
        if (strcmp(current_alias->name, name) == 0)
            return current_alias;
        current_alias = current_alias->next;
    }
    return NULL;
}

void print_aliases()
{
    Alias *current_alias = aliases;
    while (current_alias != NULL) {
        printf("%s='%s'\n", current_alias->name, current_alias->value);
        current_alias = current_alias->next;
    }
}

void add_alias(char *name, char *value)
{
    Alias *alias = find_alias(name);

    if (alias != NULL) {
        // Update the value of an existing alias
        free(alias->value);
        alias->value = strdup(value);
    } else {
        // Create a new alias
        Alias *new_alias = (Alias *)malloc(sizeof(Alias));
        new_alias->name = strdup(name);
        new_alias->value = strdup(value);
        new_alias->next = aliases;
        aliases = new_alias;
    }
}

char *expand_alias(char *command)
{
    static char expanded_command[BUFFER_SIZE];
    expanded_command[0] = '\0';

    char *token = strtok(command, " ");
    while (token != NULL) {
        Alias *alias = find_alias(token);
        if (alias != NULL) {
            strcat(expanded_command, alias->value);
        } else {
            strcat(expanded_command, token);
        }
        strcat(expanded_command, " ");
        token = strtok(NULL, " ");
    }

    return expanded_command;
}

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
        if (setenv(arguments[1], arguments[2], 1

