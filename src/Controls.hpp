#pragma once

#include <map>
#include <SFML/Window/Keyboard.hpp>
#include <string>

namespace Game {

enum class Control {
	UP, DOWN, LEFT, RIGHT, BOMB
};

extern std::map<Control, sf::Keyboard::Key> playerControls[];

inline Control controlFromString(const std::string& name) {
	if (name == "up" || name == "UP") return Control::UP;
	else if (name == "down" || name == "DOWN") return Control::DOWN;
	else if (name == "left" || name == "LEFT") return Control::LEFT;
	else if (name == "right" || name == "RIGHT") return Control::RIGHT;
	return Control::BOMB;
}

}
