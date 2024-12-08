#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "shell_functions.h"
#define MAX_INPUT_SIZE 1024
#define MAX_PATH_SIZE 1024



void handle_command(int argc, char *argv[]){
    if (argc < 1) {
        fprintf(stderr, "No command provided.\n");
        return;
    }
    if (strcmp(argv[0], "cd") == 0) {
        if (argc < 2) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(argv[1]) == 0) {
            printf("Changed directory to: %s\n", argv[1]);
        } else {
            perror("cd failed");
        }
    }
    else if (strcmp(argv[0], "pwd") == 0) {
        char cwd[MAX_PATH_SIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd failed");
        }
    }
    else if (strcmp(argv[0], "set") == 0) {
         if (argc < 3) {
            fprintf(stderr, "set: missing argument(s)\n");
        } else if (setenv(argv[1], argv[2], 1) == 0) {
            printf("Set %s=%s\n", argv[1], argv[2]);
        } else {
            perror("set failed");
        }
    }
    else if (strcmp(argv[0], "unset") == 0) {
        if (argc < 2) {
            fprintf(stderr, "unset: missing argument\n");
        } else if (unsetenv(argv[1]) == 0) {
            printf("Unset %s\n", argv[1]);
        } else {
            perror("unset failed");
        }
    }
    else if (strcmp(argv[0], "echo") == 0) {
        char buffer[MAX_INPUT_SIZE] = {0};
        for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '$') {
            char *var_name = argv[i] + 1; // Skip the '$'
            char *value = getenv(var_name);
            if (value) {
                if (strlen(buffer) + strlen(value) + 1 < MAX_INPUT_SIZE) {
                    strcat(buffer, value);
                } else {
                    fprintf(stderr, "echo: buffer overflow\n");
                    return;
                }
            } else {
                // Leave placeholder or empty space if the variable doesn't exist
                if (strlen(buffer) + 1 < MAX_INPUT_SIZE) {
                    strcat(buffer, " ");
                } else {
                    fprintf(stderr, "echo: buffer overflow\n");
                    return;
                }
            }
        } else {
            if (strlen(buffer) + strlen(argv[i]) + 1 < MAX_INPUT_SIZE) {
                strcat(buffer, argv[i]);
            } else {
                fprintf(stderr, "echo: buffer overflow\n");
                return;
            }
        }
        if (i < argc - 1) {
            strcat(buffer, " ");
        }
    }
    printf("%s\n", buffer);
}
}
