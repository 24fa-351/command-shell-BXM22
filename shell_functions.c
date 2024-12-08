#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#define MAX_INPUT_SIZE 1024
#define MAX_TOKENS 100


//redirect
void parse_arguments(const char *input, char **args) {
    int i = 0;
    char *token = strtok(strdup(input), " ");
    printf("Parsing arguments:\n"); // Debug
    while (token != NULL) {
        args[i++] = token;
        printf("Token[%d]: %s\n", i - 1, token); // Debug
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
    printf("Finished parsing arguments.\n"); // Debug
}


void execute_command(char **args) {
    if (execvp(args[0], args) < 0) {
        perror("Command execution failed");
        exit(EXIT_FAILURE);
    }
}

//cmd
void handle_redirection(char **args) {
    printf("Handling redirection...\n"); // Debug
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
        printf("Redirecting input from file: %s\n", input_file); // Debug
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
        printf("Redirecting output to file: %s\n", output_file); // Debug
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



void handle_command(int argc, char *args[]) {
    printf("Running command: %s\n", args[0]);  // Debugging line
    for (int i = 0; args[i] != NULL; i++) {
        printf("Argument %d: %s\n", i, args[i]);  // Debugging line
    }

    if (execvp(args[0], args) < 0) {
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
}

