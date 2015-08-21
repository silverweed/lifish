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

# Add git commit information (thanks, hamcha)
include(FindGit OPTIONAL)
if(GIT_FOUND)
    execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${GIT_EXECUTABLE} describe --always --long --dirty
        OUTPUT_VARIABLE CURRENTREV
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "(-0)?-[^-]+((-dirty)?)$" "\\2" CURRENTREV "${CURRENTREV}")
else()
    set(CURRENTREV "unknown")
endif()

add_definitions (-DVERSION="${MAJOR}.${MINOR}" -DCOMMIT="${CURRENTREV}")

find_package(SFML REQUIRED graphics window audio system)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})

target_compile_features(lifish PRIVATE cxx_range_for)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -Wall -pedantic -fuse-ld=gold")
if(MSYS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()
