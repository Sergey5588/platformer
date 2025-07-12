.PHONY: run all

all:
	gcc main.c -lraylib -lX11 -lGL -lm -lbox2d  -I./include -o game

debug:
	gcc main.c -g -lraylib -lX11 -lGL -lm -lbox2d  -I./include -o game-debug

run: all
	./game

test:
	gcc test.c -lGL -lraylib -lX11 -lbox2d && ./a.out
