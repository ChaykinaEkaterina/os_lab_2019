#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"


int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);  //преобразуем строку с аргументом в число

            // error handling
            if(seed <= 0)
            {
                printf("seed is a positive number\n");
                return 1;
            }
            break;

          case 1:
            array_size = atoi(optarg); //размер массива

            // error handling
            if(array_size <= 0)
            {
            printf("array_size is a positive number\n");
            return 1;
            }
            break;

          case 2:
            pnum = atoi(optarg);

            // error handling
            if(pnum <= 0)
            {
            printf("pnum is a positive number\n");
            return 1;
            }
            break;

          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':  //использование файлов
        with_files = true;
        break;

      case '?':  // найдена недействительная опция или опция не распознана 
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;


 //создание pipe
  int fd[2];
  pipe(fd);
  //fd[0]-указывает на конец канала для чтения
  //fg[1]-указывает на конец канала для записи

  struct timeval start_time;
  gettimeofday(&start_time, NULL);


//разбиваем массив на pnum отрезков 
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process

        //задаем границы i-го отрезка
        int begin = i*(array_size/pnum);   
        int end = (i+1 == pnum) ? array_size : begin + array_size/pnum;

        //ищем на этом отрезке минимум и максимум
        struct MinMax min_max = GetMinMax(array,begin,end);

        // parallel somehow

        if (with_files) {
          // use files here
          FILE *fin;
          fin = fopen("output.txt", "a+"); //открытие для чтения и дописывания в конец (создается, если не существовало)
          fprintf(fin, "%d %d\n", min_max.min, min_max.max); //записываем в файл минимальное и максимальное на отрезке
          fclose(fin);
        } else {
          // use pipe here
          write(fd[1],&min_max.max,sizeof(int));
          write(fd[1],&min_max.min,sizeof(int));
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    // your code here
    wait(NULL); //NULL т.к. нам не нужно запоминать информацию о статусе завершения процесса

    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  FILE *fin;
  fin = fopen("output.txt", "a+");

  for (int i = 0; i < pnum; i++) {
    int min ; //= INT_MAX;
    int max ; //= INT_MIN;

    if (with_files) {
      // read from files
      //FILE *fin;
      //fin = fopen("output.txt", "r"); //открываем для чтения
      fscanf(fin, "%d %d\n", &min, &max); //считываем пары минимальное-максимальное
      //printf("%d  %d\n",min, max);
      
    } else {
      // read from pipes
      read(fd[0], &max, sizeof(int));
      read(fd[0], &min, sizeof(int));
    }
    
    //сравнивая, находим минимум и максимум среди минимумов и максимумов отрезков
    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }
  fclose(fin);
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  close(fd[0]);
  close(fd[1]);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
