CC := c++
CFLAGS := -std=c++11 -Wall -Wextra -pedantic -ggdb
LDFLAGS := -lsfml-window -lsfml-graphics -lsfml-system -lsfml-audio 

test_level: Killable.o Animation.o Bomb.o AnimatedSprite.o Animated.o EntityType.o GameCache.o Level.o test_level.o LevelSet.o LevelNumText.o Music.o Game.o Texture.o ShadedText.o Track.o utils.o Entity.o EntityGroup.o Coin.o AxisMoving.o Moving.o Player.o Lifed.o Controls.o Enemy.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o test_level

Killable.o: src/Killable.cpp
	$(CC) $(CFLAGS) -c $<
Animation.o: src/Animation.cpp
	$(CC) $(CFLAGS) -c $<
Bomb.o: src/Bomb.cpp
	$(CC) $(CFLAGS) -c $<
AnimatedSprite.o: src/AnimatedSprite.cpp
	$(CC) $(CFLAGS) -c $<
Animated.o: src/Animated.cpp
	$(CC) $(CFLAGS) -c $<
EntityType.o: src/EntityType.cpp
	$(CC) $(CFLAGS) -c $<
GameCache.o: src/GameCache.cpp
	$(CC) $(CFLAGS) -c $<
Level.o: src/Level.cpp
	$(CC) $(CFLAGS) -c $<
test_level.o: src/test_level.cpp
	$(CC) $(CFLAGS) -c $<
LevelSet.o: src/LevelSet.cpp
	$(CC) $(CFLAGS) -c $<
LevelNumText.o: src/LevelNumText.cpp
	$(CC) $(CFLAGS) -c $<
Music.o: src/Music.cpp
	$(CC) $(CFLAGS) -c $<
Game.o: src/Game.cpp
	$(CC) $(CFLAGS) -c $<
Texture.o: src/Texture.cpp
	$(CC) $(CFLAGS) -c $<
ShadedText.o: src/ShadedText.cpp
	$(CC) $(CFLAGS) -c $<
Track.o: src/Track.cpp
	$(CC) $(CFLAGS) -c $<
utils.o: src/utils.cpp
	$(CC) $(CFLAGS) -c $<
Entity.o: src/Entity.cpp
	$(CC) $(CFLAGS) -c $<
EntityGroup.o: src/EntityGroup.cpp 
	$(CC) $(CFLAGS) -c $<
Coin.o: src/Coin.cpp
	$(CC) $(CFLAGS) -c $<
AxisMoving.o: src/AxisMoving.cpp
	$(CC) $(CFLAGS) -c $<
Moving.o: src/Moving.cpp
	$(CC) $(CFLAGS) -c $<
Player.o: src/Player.cpp
	$(CC) $(CFLAGS) -c $<
Lifed.o: src/Lifed.cpp
	$(CC) $(CFLAGS) -c $<
Controls.o: src/Controls.cpp
	$(CC) $(CFLAGS) -c $<
Enemy.o: src/Enemy.cpp
	$(CC) $(CFLAGS) -c $<
