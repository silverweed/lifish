#pragma once

/** General-purpose game setup */
#include <string>
#include <random>
#include <sstream>
#include <SFML/System.hpp>

// Fallback in case the game wasn't compiled properly with cmake
#ifndef VERSION
#	define VERSION "unknown"
#endif
#ifndef COMMIT
#	define COMMIT "unknown"
#endif

namespace Game {

struct Options;
class MusicManager;
class GameCache;

/****************************************************************************/
/*                         GLOBAL DEFINITIONS                               */
/****************************************************************************/

#ifdef SFML_SYSTEM_WINDOWS
constexpr char DIRSEP = '\\';
#else
constexpr char DIRSEP = '/';
#endif

constexpr unsigned short TILE_SIZE = 32; // pixels

constexpr auto PI = 3.141592653589793238L;
constexpr unsigned short PWD_BUFSIZE = 512;

/** Threshold value to consider an input from joystick getAxisPosition(). */
constexpr short JOYSTICK_INPUT_THRESHOLD = 50;

/****************************************************************************/
/*                         GLOBAL VARIABLES                                 */
/****************************************************************************/

/** The executable working directory */
extern char pwd[PWD_BUFSIZE];

/** Global game cache (caches textures in memory for faster loading) */
extern Game::GameCache cache;

/** Random number generator */
extern std::default_random_engine rng;

/** The game options */
extern Game::Options options;

/** Pointer to an unowned MusicManager, which *MUST* be created in the main function.
 *  Allows easy access to the music manager throughout the code, but the object
 *  itself has automatic lifetime and is guaranteed to die before the main's exit.
 */
extern Game::MusicManager *musicManager;

/** If true, the game exits after the current loop */
extern bool terminated;
extern int exitCode;

/****************************************************************************/
/*                         GLOBAL FUNCTIONS                                 */
/****************************************************************************/

inline std::string getAssetDir(const std::string& dir) {
	std::stringstream ss;
	ss << pwd << DIRSEP << "assets" << DIRSEP << dir;
	return ss.str();
}

inline std::string getAsset(const std::string& dir, const std::string& file) {
	return getAssetDir(dir) + DIRSEP + file;
}

/** Initializes runtime variables */
bool initCore();

}
