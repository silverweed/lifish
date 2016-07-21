#pragma once

/** General-purpose game setup */
#include <string>
#include <random>
#include <sstream>
#include <SFML/System.hpp>
#include "GameCache.hpp"

// Fallback in case the game wasn't compiled properly with cmake
#ifndef VERSION
#	define VERSION "unknown"
#endif
#ifndef COMMIT
#	define COMMIT "unknown"
#endif

namespace Game {

struct Options;

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
extern GameCache cache;

/** Random number generator */
extern std::default_random_engine rng;

/** The game options */
extern Options options;

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
