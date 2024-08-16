INCLUDE_DIRS = -I include/SDL2
LIB_DIRS = -L lib -l SDL2-2.0.0

SRC = $(wildcard src/*.c)

all:
	gcc -g $(SRC) -o build/fractal $(INCLUDE_DIRS) $(LIB_DIRS)
