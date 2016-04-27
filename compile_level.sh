cd src
clang++ -O0 -ggdb -std=c++11 Killable.cpp Animation.cpp Bomb.cpp AnimatedSprite.cpp Animated.cpp EntityType.cpp GameCache.cpp Level.cpp test_level.cpp LevelSet.cpp LevelNumText.cpp Music.cpp Game.cpp Texture.cpp ShadedText.cpp Track.cpp utils.cpp Entity.cpp -lsfml-window -lsfml-graphics -lsfml-system -lsfml-audio -o ../test_level
cd ..
