#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
  pid_t pid=fork();
  if (pid==0) //дочерний процесс 
  {
      execvp("sequential_min_max",argv);
  }
  return 0;
}