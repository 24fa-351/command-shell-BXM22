#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "shell_functions.h"

#define MAX_INPUT_SIZE 1024
#define MAX_TOKENS 100

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <outputFile> <command> [<arg1>, <arg2> ...]\n", argv[0]);
        return 1;
    }

    char *output_file = argv[1];
    char *command = argv[2];
    char *args[MAX_TOKENS];

    args[0] = command;
    for (int i = 3; i < argc; i++) {
        args[i - 2] = argv[i];
    }
    args[argc - 2] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
        int out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd < 0) {
            perror("Failed to open output file");
            exit(EXIT_FAILURE);
        }

        if (dup2(out_fd, STDOUT_FILENO) < 0) {
            perror("Failed to redirect output");
            close(out_fd);
            exit(EXIT_FAILURE);
        }
        close(out_fd);

        if (strchr(command, '|') != NULL) {
            handle_pipes(command);
        } else {
            handle_command(argc - 2, args);
        }

        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }

    return 0;
}
