#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell_functions.h"

#define MAX_INPUT_SIZE 1024
#define MAX_TOKENS 100



int main() {
    char input[MAX_INPUT_SIZE];
    char *argv[MAX_TOKENS];
    int argc;

    while (1) {
        printf("xsh# "); 
        if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
            break; 
        }
        argc = 0;
        char *token = strtok(input, " \t\n");
        while (token && argc < MAX_TOKENS - 1) {
            argv[argc++] = token;
            token = strtok(NULL, " \t\n");
        }
        argv[argc] = NULL;

        if (argc > 0 && (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0)) {
            break;
        }
        handle_command(argc, argv);
    }

    return 0;
}
