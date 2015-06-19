CC = g++
CFLAGS = -std=c++14 -Wall
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = lifish

SRCS = main.cpp Level.cpp LevelSet.cpp GameCache.cpp Game.cpp Entity.cpp EntityType.cpp 

OBJS = $(SRCS:.c=.o)

.PHONY: clean

all: $(TARGET) 

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean: 
	$(RM) *.o *~ $(TARGET)


#$(TARGET): main.o Level.o LevelSet.o Game.o GameCache.o Entity.o EntityType.o 
#	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
#
#main.o: main.cpp Level.hpp Game.hpp GameCache.hpp Drawable.hpp Entity.hpp MovingEntity.hpp FixedEntity.hpp EntityType.hpp utils.hpp
#	$(CC) $(CFLAGS) -c $<
#Level.o: Level.cpp Level.hpp Game.hpp GameCache.hpp Drawable.hpp Entity.hpp MovingEntity.hpp FixedEntity.hpp EntityType.hpp utils.hpp
#	$(CC) $(CFLAGS) -c $<
#LevelSet.o: LevelSet.cpp LevelSet.hpp Game.hpp Level.hpp EntityType.hpp json.hpp
#	$(CC) $(CFLAGS) -c $<
#GameCache.o: GameCache.cpp GameCache.hpp Game.hpp
#	$(CC) $(CFLAGS) -c $<
#Game.o: Game.cpp Game.hpp GameCache.hpp
#	$(CC) $(CFLAGS) -c $<
#Entity.o: Entity.cpp Entity.hpp Drawable.hpp
#	$(CC) $(CFLAGS) -c $<
#EntityType.o: EntityType.cpp EntityType.hpp 
#	$(CC) $(CFLAGS) -c $<
#FixedEntity.o: FixedEntity.cpp FixedEntity.hpp Entity.hpp Drawable.hpp
#	$(CC) $(CFLAGS) -c $<
#MovingEntity.o: MovingEntity.cpp MovingEntity.hpp Entity.hpp Drawable.hpp
#	$(CC) $(CFLAGS) -c $<
#
#
#
#.PHONY: clean
#clean:
#	rm -f *.o *~
