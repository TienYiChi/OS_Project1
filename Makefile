CC=gcc
CFLAGS=-I.
DEPS = main.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: fork.o sched_algo.o 
	$(CC) -o fork.out fork.o sched_algo.o 