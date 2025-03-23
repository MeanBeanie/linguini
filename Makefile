CC = gcc
NAME = tests
SRCS = linguini.c linguini_Freetype.c main.c
CFLAGS = -o $(NAME) --std=c11 -D_GNU_SOURCE=1 -lm $(shell pkg-config freetype2 --cflags --libs)

ifdef USE_SDL
	CFLAGS += $(shell pkg-config sdl2 --cflags --libs) -DUSE_SDL
	SRCS += linguini_SDL.c
endif

ifdef USE_X11
	CFLAGS += $(shell pkg-config x11 --cflags --libs) -DUSE_X11
	SRCS += linguini_X11.c
endif

ifdef USE_WAYLAND
	CFLAGS += -lwayland-client -DUSE_WAYLAND -lrt
	SRCS += xdg-shell-protocol.c linguini_Wayland.c
endif

debug:
	$(CC) $(SRCS) $(CFLAGS) -g
release:
	$(CC) $(SRCS) $(CFLAGS)
