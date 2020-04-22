CC=gcc
CFLAGS=-I.

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: fork.o sched_algo.o 
	$(CC) -o fork.out fork.o sched_algo.o 