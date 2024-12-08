#ifndef SHELL_FUNCTIONS_H
#define SHELL_FUNCTIONS_H

void parse_arguments(const char *input, char **args);
void handle_redirection(char **args);
void handle_pipes(char *command);
void execute_command(char **args);
void handle_command(int argc, char *argv[]);


#endif