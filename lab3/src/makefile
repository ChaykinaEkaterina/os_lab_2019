# переменная CC указывает компилятор, используемый для сборки
# в переменной CFLAGS лежат флаги, которые передаются компилятору

CC=gcc
CFLAGS=-I.

all: sequential_min_max parallel_min_max utils.o find_min_max.o task5

sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

task5 : utils.o utils.h find_min_max.o find_min_max.h sequential_min_max.o
	$(CC) -o task5 utils.o find_min_max.o  sequential_min_max.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max task5 sequential_min_max.o



