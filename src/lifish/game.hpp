/*!
 * @mainpage
 *
 * Lifish is an open source game based on "BOOM", a shareware MacOS game by
 * Federico Filipponi.
 *
 * @copyright This game is licensed under the Lifish License, available at
 *            https://silverweed.github.io/lifish-license.txt
 *            or in the LICENSE file in this repository's root directory.
 *
 * @author [silverweed](https://github.com/silverweed)
 * @see https://github.com/silverweed/lifish to download the source code
*/
#pragma once

#include "core.hpp"
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

/**
 * Game-specific definitions, global functions and variables
 */
namespace lif {

/****************************************************************************/
/*                         GLOBAL DEFINITIONS                               */
/****************************************************************************/

constexpr auto MAX_PLAYERS = 2;
/** Width of the game viewport, in pixels */
constexpr auto GAME_WIDTH = 544;
/** Height of the game viewport, in pixels */
constexpr auto GAME_HEIGHT = 480;

constexpr auto SIDE_PANEL_WIDTH = 96;
constexpr auto SIDE_PANEL_HEIGHT = 480;

constexpr auto WINDOW_WIDTH = SIDE_PANEL_WIDTH + GAME_WIDTH;
constexpr auto WINDOW_HEIGHT = GAME_HEIGHT;

/** How much the main game window is shifted relative to the
 *  SFML window (horizontally)
 */
constexpr auto MAIN_WINDOW_SHIFT = 1 + SIDE_PANEL_WIDTH;

constexpr auto N_ENEMIES = 10;

namespace fonts {
	constexpr auto POINTS          = "pf_tempesta_seven_bold.ttf";
	constexpr auto INTERLEVEL      = "pf_tempesta_seven_bold.ttf";
	constexpr auto LEVELNUM        = "pf_tempesta_seven.ttf";
	constexpr auto CUTSCENES       = "pf_tempesta_seven.ttf";
	constexpr auto DEBUG_INFO      = "pf_tempesta_seven_condensed.ttf";
	constexpr auto SIDE_PANEL      = "pf_tempesta_seven_bold.ttf";
	constexpr auto SIDE_PANEL_THIN = "pf_tempesta_seven_condensed_bold.ttf";
	constexpr auto SIDE_PANEL_MONO = "Hack-Regular.ttf";
	constexpr auto SCREEN          = "pf_tempesta_seven_bold.ttf";
}

constexpr auto HURRY_UP_SOUND    = "hurryup.ogg";
constexpr auto GAME_OVER_SOUND   = "gameover.ogg";
constexpr auto EXTRA_GAME_SOUND  = "extragame.ogg";
constexpr auto EXTRA_LIFE_SOUND  = "extralife.ogg";
constexpr auto LEVEL_CLEAR_SOUND = "levelclear.ogg";
constexpr auto TIME_BONUS_SOUND  = "timebonus.ogg";

/****************************************************************************/
/*                  GLOBAL VARIABLES AND FUNCTIONS                          */
/****************************************************************************/

bool init();

std::string gameInfo();

class HighScoreManager& getHighScoreManager();


} // end namespace lif
