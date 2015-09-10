#pragma once
/**
 * Game definitions, global functions and variables
 */

#include <string>
#include <map>
#include <sstream>
#include <random>
#include <SFML/Graphics.hpp>
#include "GameCache.hpp"

class LoopingMusic;

namespace Game {

/****************************************************************************/
/*                         GLOBAL DEFINITIONS                               */
/****************************************************************************/

#ifdef SFML_SYSTEM_WINDOWS
constexpr char DIRSEP = '\\';
#else
constexpr char DIRSEP = '/';
#endif

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
constexpr unsigned short INITIAL_CONTINUES = 3;

constexpr static unsigned short SIDE_PANEL_WIDTH = 96;
constexpr static unsigned short SIDE_PANEL_HEIGHT = 480;

constexpr static unsigned short WINDOW_WIDTH = SIDE_PANEL_WIDTH + (LEVEL_WIDTH + 2) * TILE_SIZE;
constexpr static unsigned short WINDOW_HEIGHT = SIDE_PANEL_HEIGHT;

/** How much the main game window is shifted relative to the
 *  SFML window (horizontally)
 */
constexpr static float MAIN_WINDOW_SHIFT = 1 + SIDE_PANEL_WIDTH;

/** The number of letters needed to get an extra life */
constexpr static unsigned short N_EXTRA_LETTERS = 5;

constexpr unsigned short PWD_BUFSIZE = 512;

constexpr auto PI = 3.141592653589793238L;

namespace Fonts {
	constexpr static auto POINTS = "pf_tempesta_seven_bold.ttf";
	constexpr static auto INTERLEVEL = "pf_tempesta_seven_bold.ttf";
	constexpr static auto LEVELNUM = "pf_tempesta_seven.ttf";
	constexpr static auto DEBUG_INFO = "pf_tempesta_seven_condensed.ttf";
	constexpr static auto SIDE_PANEL = "pf_tempesta_seven_bold.ttf";
	constexpr static auto SCREEN = "pf_tempesta_seven_bold.ttf";
}

/** Indexing convention for Game::Sounded entities */
namespace Sounds {
	/** Enemy's, player's, boss's death, bullet's hit */
	constexpr static unsigned short DEATH  = 0;
	/** Bullet's shooting sound */
	constexpr static unsigned short SHOT   = 1;
	/** Enemy's yell */
	constexpr static unsigned short YELL   = 1;
	/** Boss's / Player's hurt sound */
	constexpr static unsigned short HURT   = 1;
	/** Player's win sound */
	constexpr static unsigned short WIN    = 2;
	/** Enemy's attack sound */
	constexpr static unsigned short ATTACK = 2;
}

constexpr auto HURRY_UP_SOUND    = "hurryup.ogg";
constexpr auto GAME_OVER_SOUND   = "gameover.ogg";
constexpr auto EXTRA_GAME_SOUND  = "extragame.ogg";
constexpr auto EXTRA_LIFE_SOUND  = "extralife.ogg";
constexpr auto LEVEL_CLEAR_SOUND = "levelclear.ogg";
constexpr auto TIME_BONUS_SOUND  = "timebonus.ogg";

/****************************************************************************/
/*                         GLOBAL VARIABLES                                 */
/****************************************************************************/

/** The executable working directory */
extern char pwd[PWD_BUFSIZE];

/** Global game cache (caches textures in memory for faster loading) */
extern GameCache cache;

/** Random number generator */
extern std::default_random_engine rng;

/** The players' score */
extern std::array<unsigned int, MAX_PLAYERS> score;

/** The music */
extern LoopingMusic *music;

/** The music volume */
extern float music_volume;

/** The FX volume */
extern float sounds_volume;

/** The remaining of 'continues' per player */
extern std::array<unsigned short, MAX_PLAYERS> playerContinues;

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
bool init();

} // end namespace Game
