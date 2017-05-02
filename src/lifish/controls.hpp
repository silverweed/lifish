#pragma once

#include <unordered_map>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <array>
#include "game.hpp"

namespace lif {

namespace controls {

enum Control {
	CTRL_UP    = 0,
	CTRL_DOWN  = 1,
	CTRL_LEFT  = 2,
	CTRL_RIGHT = 3,
	CTRL_BOMB  = 4
};

constexpr static unsigned short CONTROLS_NUM = 5;

extern std::array<sf::Keyboard::Key, CONTROLS_NUM> players[lif::MAX_PLAYERS];
extern std::array<unsigned, lif::MAX_PLAYERS> joystickBombKey;
/** -1 if i-th player doesn't use joystick, else the joystick index. */
extern std::array<short, lif::MAX_PLAYERS> useJoystick;

namespace {
	static const std::unordered_map<std::string, Control> ctrlStrings = {
		{ "up", CTRL_UP },
		{ "UP", CTRL_UP },
		{ "down", CTRL_DOWN },
		{ "DOWN", CTRL_DOWN },
		{ "left", CTRL_LEFT },
		{ "LEFT", CTRL_LEFT },
		{ "right", CTRL_RIGHT },
		{ "RIGHT", CTRL_RIGHT },
	};
}

inline Control fromString(const std::string& name) {
	const auto it = ctrlStrings.find(name);
	if (it == ctrlStrings.end()) return CTRL_BOMB;
	return it->second;
}

} // end namespace controls

} // end namespace lif
