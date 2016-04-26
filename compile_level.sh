cd src
g++ -O0 -ggdb -std=c++11 EntityType.cpp GameCache.cpp Level.cpp test_level.cpp LevelSet.cpp LevelNumText.cpp Music.cpp Game.cpp Texture.cpp ShadedText.cpp Track.cpp Entity.cpp -lsfml-window -lsfml-graphics -lsfml-system -lsfml-audio -o ../test_level
cd ..
