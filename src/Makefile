CC := g++
CFLAGS := -std=c++14 -Wall
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system

all: main

main: main.o Level.o LevelSet.o Game.o GameCache.o Entity.o EntityType.o 
	$(CC) $(CFLAGS) -o lifish $^ $(LDFLAGS)

Level.o: Game.hpp GameCache.hpp Drawable.hpp Entity.hpp MovingEntity.hpp FixedEntity.hpp EntityType.hpp utils.hpp
LevelSet.o: Game.hpp Level.hpp EntityType.hpp json.hpp
GameCache.o: Game.hpp
Game.o: GameCache.hpp
Entity.o: Drawable.hpp

%.o: %.cpp Game.hpp Drawable.hpp Level.hpp LevelSet.hpp GameCache.hpp Entity.hpp
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o *~
