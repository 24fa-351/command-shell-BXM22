#include <stdlib.h>
#include <stdio.h>
#include "shell_functions.h"

void shell_loop(void){
    char *line;
    char **args;
    int status;
    
    do {
        printf("> ");
        line = read_line();
        args = split_line(line);
        status = execute(args);
    
        free(line);
        free(args);
    } while (status);
}
char *read_line(void){
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}
char **split_line(char *line){
    
}
