#!/bin/bash
# COMPILE LIFISH FIRST!
set -x
${CXX:-g++} -DSFML_STATIC=1 -std=c++11 -I/local/include -L/local/lib -I../src/{core{,/collisions,/components,/cutscenes,/entities,/input,/debug},ui,lifish{,/entities,/components,/debug,/level},third_party} $1  ../CMakeFiles/lifish.dir/src/{core{,/collisions,/components,/cutscenes,/entities,/input,/debug},lifish{,/entities,/components,/debug,/level,/conf},third_party,ui}/*obj  /local/lib/lib{sfml-{audio,graphics,window,system}-s,FLAC,jpeg,freetype,vorbis{enc,file,},ogg}.a  -o ${1%.cpp}.x -lopenal32 -lgdi32 -lopengl32 -lglu32 -lwinmm
