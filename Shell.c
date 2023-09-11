#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

void tokenize(char *input, int *token_count, char ***token_vector) {
    *token_count = 0;
    int max_tokens = 100;
    *token_vector = (char **)malloc(sizeof(char *) * (max_tokens + 1)); // +1 for the NULL pointer

    char *token = strtok(input, " \t"); // Use strtok to tokenize by spaces and tabs
    while (token != NULL) {
        if (*token_count >= max_tokens) {
            // Handle the case where you exceed the allocated token_vector size
            break;
        }

        (*token_vector)[*token_count] = strdup(token); // Allocate and copy the token
        (*token_count)++;
        token = strtok(NULL, " \t");
    }
    (*token_vector)[*token_count] = NULL; // Mark the end of tokens
}

int main() {
    char input[1024]; // Buffer for user input

    while (1) {
        printf("Shell> "); // Print a simple prompt
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // End of input (Ctrl-D)
            break;
        }

        // Remove newline character from input
        input[strcspn(input, "\n")] = '\0';

        // Ignore blank lines
        if (input[0] == '\0') {
            continue;
        }

        int token_count;
        char **token_vector;
        tokenize(input, &token_count, &token_vector);

        int pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            // Child process
            if (execvp(token_vector[0], token_vector) == -1) {
                perror("execvp");
                exit(1);
            }
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }

        // Free memory for tokens
        for (int i = 0; i < token_count; i++) {
            free(token_vector[i]);
        }
        free(token_vector);
    }

    return 0;
}
