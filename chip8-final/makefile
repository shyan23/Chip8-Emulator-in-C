CFlags=-std=c17 -Wall -Wextra -Werror -g

all:
	gcc menu.c -o menu $(CFlags) `sdl2-config --cflags --libs` -lSDL2 -lSDL2_ttf
	gcc chip8.c -o chip8 $(CFlags) `sdl2-config --cflags --libs`

debug:
	gcc menu.c -o menu $(CFlags) `sdl2-config --cflags --libs` -lSDL2 -SDL2_ttf -DEDEBUG
	gcc chip8.c -o chip8 $(CFlags) `sdl2-config --cflags --libs` -DEDEBUG
