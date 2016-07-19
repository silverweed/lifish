#pragma once

#include <unordered_map>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <array>
#include "game.hpp"

namespace Game {

namespace Controls {

enum Control : unsigned short {
	CTRL_UP    = 0,
	CTRL_DOWN  = 1,
	CTRL_LEFT  = 2,
	CTRL_RIGHT = 3,
	CTRL_BOMB  = 4
};

constexpr static unsigned short CONTROLS_NUM = 5;

extern std::array<sf::Keyboard::Key, CONTROLS_NUM> players[Game::MAX_PLAYERS];
extern std::array<unsigned int, Game::MAX_PLAYERS> joystickBombKey;

inline Control fromString(const std::string& name) {
	if (name == "up" || name == "UP") return CTRL_UP;
	else if (name == "down" || name == "DOWN") return CTRL_DOWN;
	else if (name == "left" || name == "LEFT") return CTRL_LEFT;
	else if (name == "right" || name == "RIGHT") return CTRL_RIGHT;
	return CTRL_BOMB;
}

} // end namespace Controls

} // end namespace Game
