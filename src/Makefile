CC := clang++
CFLAGS := -std=c++11 -Wall
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system

all: main

main: main.o Level.o Tile.o LevelSet.o
	$(CC) $(CFLAGS) -o lifish $^ $(LDFLAGS)

%.o: %.cpp Game.hpp Drawable.hpp Level.hpp Tile.hpp LevelSet.hpp
	$(CC) $(CFLAGS) -c $<
