#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "shell_functions.h"
#define MAX_INPUT_SIZE 1024


void run_comandline(void){
    char input[MAX_INPUT_SIZE];

    printf("xsh# ");

    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            break;
        }
    }
    printf("Goodbye!\n");
}