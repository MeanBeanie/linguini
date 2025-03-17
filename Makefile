CC = gcc
NAME = tests
CFLAGS = -o $(NAME) --std=c11 -lm $(shell pkg-config sdl2 --cflags --libs)

debug:
	$(CC) linguini.c main.c $(CFLAGS) -g
release:
	$(CC) linguini.c main.c $(CFLAGS)
