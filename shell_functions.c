#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "shell_functions.h"

#define MAX_INPUT_SIZE 1024
#define MAX_PATH_SIZE 1024

void parse_arguments(const char *input, char **args) {
    int i = 0;
    char *token = strtok(strdup(input), " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

void handle_redirection(char **args) {
    int in_fd = -1, out_fd = -1;
    char *input_file = NULL;
    char *output_file = NULL;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            input_file = args[i + 1];
            args[i] = NULL; 
        } else if (strcmp(args[i], ">") == 0) {
            output_file = args[i + 1];
            args[i] = NULL;  
        }
    }

    if (input_file) {
        in_fd = open(input_file, O_RDONLY);
        if (in_fd < 0) {
            perror("Failed to open input file");
            exit(EXIT_FAILURE);
        }
        if (dup2(in_fd, STDIN_FILENO) < 0) {
            perror("Failed to redirect input");
            exit(EXIT_FAILURE);
        }
        close(in_fd);
    }

    if (output_file) {
        out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd < 0) {
            perror("Failed to open output file");
            exit(EXIT_FAILURE);
        }
        if (dup2(out_fd, STDOUT_FILENO) < 0) {
            perror("Failed to redirect output");
            exit(EXIT_FAILURE);
        }
        close(out_fd);
    }
}

void handle_pipes(char *command) {
    char *cmds[2];
    int pipe_fd[2];
    pid_t pid;

    cmds[0] = strtok(command, "|");
    cmds[1] = strtok(NULL, "|");

    if (cmds[1] == NULL) {
        fprintf(stderr, "Invalid pipe command\n");
        return;
    }

    if (pipe(pipe_fd) < 0) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        char *args[MAX_INPUT_SIZE];
        parse_arguments(cmds[0], args);
        execute_command(args);
        exit(EXIT_SUCCESS);
    }

    if ((pid = fork()) == 0) {
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        char *args[MAX_INPUT_SIZE];
        parse_arguments(cmds[1], args);
        execute_command(args);
        exit(EXIT_SUCCESS);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);
    wait(NULL);
    wait(NULL);
}

void execute_command(char **args) {
    if (execvp(args[0], args) < 0) {
        perror("Command execution failed");
        exit(EXIT_FAILURE);
    }
}

void handle_command(int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr, "No command provided.\n");
        return;
    }

    char *args[MAX_INPUT_SIZE];
    parse_arguments(argv[0], args);

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0) {
            handle_redirection(args); 
            break;
        }
    }

    if (strchr(argv[0], '|') != NULL) {
        handle_pipes(argv[0]);
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        execute_command(args); 
        wait(NULL);
    } else {
        perror("Fork failed");
    }
}
