#pragma once
/**
 * Game definitions
 */

#include <string>
#include <map>
#include <sstream>
#include <random>
#include <SFML/Graphics.hpp>
#include "GameCache.hpp"

namespace Game {

constexpr char DIRSEP = '/';

constexpr unsigned short TILE_SIZE = 32; // pixels
/**
 * Level width / height in tiles. Don't take the
 * level borders into accounts, so a level will
 * occupy LEVEL_WIDTH+2, LEVEL_HEIGHT+2 tiles.
 */
constexpr unsigned short LEVEL_WIDTH = 15; 
constexpr unsigned short LEVEL_HEIGHT = 13;

constexpr float DAMAGE_SHIELD_TIME = 1000; // ms
constexpr float RESURRECT_SHIELD_TIME = 5000; // ms

constexpr unsigned short MAX_PLAYERS = 2;

constexpr unsigned short INITIAL_LIVES = 3;

/** How much the main game window is shifted relative to the
 *  SFML window
 */
const sf::Vector2f MAIN_WINDOW_SHIFT = sf::Vector2f(128.f, 0.f);

constexpr unsigned short PWD_BUFSIZE = 512;
/** The executable working directory */
extern char pwd[PWD_BUFSIZE];

/** Global game cache (caches textures in memory for faster loading) */
static GameCache cache;

/** Random number generator */
static std::default_random_engine rng;

/** The players' score */
extern std::array<unsigned int, MAX_PLAYERS> score;

inline std::string getAssetDir(const std::string& dir) {
	std::stringstream ss;
	ss << pwd << DIRSEP << "assets" << DIRSEP << dir;
	return ss.str();
}

inline std::string getAsset(const std::string& dir, const std::string& file) {
	return getAssetDir(dir) + DIRSEP + file;
}

/** Initializes runtime variables */
bool init();

namespace Fonts {
	static constexpr auto POINTS = "pf_tempesta_seven_bold.ttf";
	static constexpr auto LEVELNUM = "pf_tempesta_seven.ttf";
	static constexpr auto DEBUG_INFO = "pf_tempesta_seven_condensed.ttf";
}

}
