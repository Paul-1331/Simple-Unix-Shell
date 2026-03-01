CC=gcc
CFLAGS=-Wall -Wextra

OBJS=main.o parser.o executor.o builtins.o jobs.o

myshell: $(OBJS)
	$(CC) $(CFLAGS) -o myshell $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o myshell