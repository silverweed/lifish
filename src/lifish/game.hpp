/*!
 * @mainpage
 * 
 * Lifish is an open source game based on "BOOM", a shareware MacOS game by
 * Federico Filipponi.
 * 
 * @copyright The code is licensed under the [GNU General Public 
 * 	   License](https://github.com/silverweed/lifish/blob/master/LICENSE.txt):
 * 
 * 	This program is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 * 
 * 	This program is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 * 
 * 	You should have received a copy of the GNU General Public License
 * 	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author [Giacomo Parolini](https://github.com/silverweed)
 * @see https://github.com/silverweed/lifish to download the source code
*/
#pragma once

#include <string>
#include <map>
#include <SFML/Graphics.hpp>
#include "core.hpp"

/**
 * Game-specific definitions, global functions and variables
 */
namespace lif {

/****************************************************************************/
/*                         GLOBAL DEFINITIONS                               */
/****************************************************************************/

constexpr unsigned short MAX_PLAYERS = 2;
/**
 * Level width / height in tiles. Don't take the
 * level borders into accounts, so a level will
 * occupy LEVEL_WIDTH+2, LEVEL_HEIGHT+2 tiles.
 */
constexpr unsigned short LEVEL_WIDTH = 15; 
constexpr unsigned short LEVEL_HEIGHT = 13;

constexpr unsigned short SIDE_PANEL_WIDTH = 96;
constexpr unsigned short SIDE_PANEL_HEIGHT = 480;

constexpr unsigned short WINDOW_WIDTH = SIDE_PANEL_WIDTH + (LEVEL_WIDTH + 2) * TILE_SIZE;
constexpr unsigned short WINDOW_HEIGHT = (LEVEL_HEIGHT + 2) * TILE_SIZE;

/** How much the main game window is shifted relative to the
 *  SFML window (horizontally)
 */
constexpr float MAIN_WINDOW_SHIFT = 1 + SIDE_PANEL_WIDTH;

constexpr unsigned short N_ENEMIES = 10;

namespace fonts {
	constexpr auto POINTS          = "pf_tempesta_seven_bold.ttf";
	constexpr auto INTERLEVEL      = "pf_tempesta_seven_bold.ttf";
	constexpr auto LEVELNUM        = "pf_tempesta_seven.ttf";
	constexpr auto CUTSCENES       = "pf_tempesta_seven.ttf";
	constexpr auto DEBUG_INFO      = "pf_tempesta_seven_condensed.ttf";
	constexpr auto SIDE_PANEL      = "pf_tempesta_seven_bold.ttf";
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
/*                         GLOBAL VARIABLES                                 */
/****************************************************************************/

/** The players' score */
extern std::array<unsigned int, MAX_PLAYERS> score;

/** The remaining of 'continues' per player */
extern std::array<unsigned short, MAX_PLAYERS> playerContinues;

bool init();

std::string game_info();

} // end namespace lif
