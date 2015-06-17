CC := g++
CFLAGS := -std=c++11 -Wall
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system

all: main

main: main.o Level.o Tile.o LevelSet.o Game.o GameCache.o
	$(CC) $(CFLAGS) -o lifish $^ $(LDFLAGS)

Level.o: Game.hpp Tile.hpp GameCache.hpp Drawable.hpp utils.hpp
Tile.o: Game.hpp 
LevelSet.o: Game.hpp Level.hpp json.hpp
GameCache.o: Game.hpp
Game.o: GameCache.hpp

%.o: %.cpp Game.hpp Drawable.hpp Level.hpp Tile.hpp LevelSet.hpp GameCache.hpp
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o *~
