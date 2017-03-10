#!/bin/bash
# COMPILE LIFISH FIRST!
${CXX:-g++} -std=c++11 -I../src/{core{,/collisions,/components,/cutscenes,/entities,/input,/debug},ui,lifish{,/entities,/components,/debug,/level},third_party} $1  ../CMakeFiles/lifish.dir/src/{core{,/collisions,/components,/cutscenes,/entities,/input,/debug},lifish{,/entities,/components,/debug,/level,/conf},third_party,ui}/*o  -lsfml-system -lsfml-graphics -lsfml-window -lsfml-audio -o ${1%.cpp}.x
