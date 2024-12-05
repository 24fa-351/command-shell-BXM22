#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell_functions.h"



int main(int argc, char *argv[])
{
  run_comandline();

  return EXIT_SUCCESS;
}