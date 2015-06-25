cmake_minimum_required (VERSION 3.0.2 FATAL_ERROR)
project (lifish)
set (PROJECT_NAME lifish)
set (MAJOR 0)
set (MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

file (GLOB LIFISH_HEADERS src/*.hpp)
file (GLOB LIFISH_SRC src/*.cpp)
add_executable (${PROJECT_NAME} ${LIFISH_SRC} ${LIFISH_HEADERS})
set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

add_definitions (-DVERSION=${MAJOR}.${MINOR})

find_package(SFML REQUIRED system window graphics network audio)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})

target_compile_features(lifish PRIVATE cxx_range_for)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -Wall -pedantic")
