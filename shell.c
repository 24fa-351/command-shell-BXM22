#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>



int main(int argc, char **argv)
{
  shell_loop();
  return EXIT_SUCCESS;
}