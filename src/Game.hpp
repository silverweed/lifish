#pragma once
/**
 * Game definitions
 */

namespace Game {

#if defined(__unix__) || defined(unix) || _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED || _POSIX_C_SOURCE >= 200112L
#	define IS_POSIX
#elif defined(_WIN32) || defined(_WIN64)
#	define IS_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#	define IS_APPLE
#endif

#ifdef IS_WINDOWS
constexpr char DIRSEP = '\\';
#else
constexpr char DIRSEP = '/';
#endif

constexpr unsigned short TILE_SIZE = 32;
/**
 * Level width / height in tiles. Don't take the
 * level borders into accounts, so a level will
 * occupy LEVEL_WIDTH+2, LEVEL_HEIGHT+2 tiles.
 */
constexpr unsigned short LEVEL_WIDTH = 15; 
constexpr unsigned short LEVEL_HEIGHT = 13;

constexpr unsigned short PWD_BUFSIZE = 512;
/** The executable working directory */
extern char pwd[PWD_BUFSIZE];

/** Global game cache (caches textures in memory for faster loading) */
class GameCache;
extern GameCache cache;

/** Initializes runtime variables */
bool init();

}
