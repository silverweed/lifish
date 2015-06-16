CC := clang++
CFLAGS := -std=c++11 -Wall
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system

all: main

main: main.o Level.o
	$(CC) $(CFLAGS) -o lifish $^ $(LDFLAGS)

%.o: %.cpp Game.hpp Drawable.hpp Level.hpp
	$(CC) $(CFLAGS) -c $<
