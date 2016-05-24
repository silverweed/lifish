#pragma once

#include <unordered_map>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <array>
#include "Game.hpp"

namespace Game {

enum Control : unsigned short {
	UP    = 0,
	DOWN  = 1,
	LEFT  = 2,
	RIGHT = 3,
	BOMB  = 4
};

namespace Controls {
	constexpr static unsigned short CONTROLS_NUM = 5;

	extern std::array<sf::Keyboard::Key, CONTROLS_NUM> players[Game::MAX_PLAYERS];
	extern std::array<unsigned int, Game::MAX_PLAYERS> joystickBombKey;

	inline Control fromString(const std::string& name) {
		if (name == "up" || name == "UP") return Control::UP;
		else if (name == "down" || name == "DOWN") return Control::DOWN;
		else if (name == "left" || name == "LEFT") return Control::LEFT;
		else if (name == "right" || name == "RIGHT") return Control::RIGHT;
		return Control::BOMB;
	}
}

}
