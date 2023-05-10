#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>

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
        if (token[0] == '$') {
            char *variable = token + 1;
            if (strcmp(variable, "?") == 0) {
                char exit_status[16];
                sprintf(exit_status, "%d", WEXITSTATUS(last_status));
                strcat(expanded_command, exit_status);
            } else if (strcmp(variable, "$") == 0) {
                char pid[16];
                sprintf(pid, "%d", getpid());
                strcat(expanded_command, pid);
            } else {
                char *env_value = getenv(variable);
                if (env_value != NULL) {
                    strcat(expanded_command, env_value);
                } else {
                    fprintf(stderr, "Variable %s not found\n", variable);
                    return NULL;
                }
            }
        } else {
            Alias *alias = find_alias(token);
            if (alias != NULL) {
                strcat(expanded_command, alias->value);
            } else {
                strcat(expanded_command, token);
            }
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

