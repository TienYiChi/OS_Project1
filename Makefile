CC=gcc
CFLAGS=-I.

%.o: %.c $(DEPS)
	$(CC) -O0 -c -o $@ $< $(CFLAGS)

make: fork.o sched_algo.o 
	$(CC) -O0 -o fork.out fork.o sched_algo.o 
