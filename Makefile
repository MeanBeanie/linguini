CC = gcc
NAME = tests
CFLAGS = -o $(NAME) --std=c11 -lm

debug:
	$(CC) *.c $(CFLAGS) -g
release:
	$(CC) *.c $(CFLAGS)
