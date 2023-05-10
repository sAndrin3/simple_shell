#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

char *my_getline(void)
{
    static char buffer[BUFFER_SIZE];
    static int buffer_index = 0;
    static int bytes_read = 0;
    
    while (1) {
        if (buffer_index >= bytes_read) {
            bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            if (bytes_read <= 0) {
                return NULL;
            }
            buffer_index = 0;
        }
        
        char current_char = buffer[buffer_index];
        buffer_index++;
        
        if (current_char == '\n') {
            buffer_index--; // Exclude the newline character from the line
            buffer[buffer_index] = '\0'; // Replace newline with null terminator
            return buffer;
        }
    }
}

int main(void)
{
    char *prompt = "$ ";
    char *line;
    
    while (1) {
        printf("%s", prompt);
        line = my_getline();

        if (line == NULL) {
            printf("\n");
            break;
        } else {
            printf("Received input: %s\n", line);
        }
    }

    return 0;
}

