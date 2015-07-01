#pragma once

#include <map>
#include <SFML/Window/Keyboard.hpp>

namespace Game {

enum class Control {
	UP, DOWN, LEFT, RIGHT, BOMB
};

std::map<Control, sf::Keyboard::Key> playerControls[] = {
	{
		{ Control::UP,    sf::Keyboard::Key::Up },
		{ Control::DOWN,  sf::Keyboard::Key::Down },
		{ Control::LEFT,  sf::Keyboard::Key::Left },
		{ Control::RIGHT, sf::Keyboard::Key::Right },
		{ Control::BOMB,  sf::Keyboard::Key::RControl }
	},
	{
		{ Control::UP,    sf::Keyboard::Key::W },
		{ Control::DOWN,  sf::Keyboard::Key::S },
		{ Control::LEFT,  sf::Keyboard::Key::A },
		{ Control::RIGHT, sf::Keyboard::Key::D },
		{ Control::BOMB,  sf::Keyboard::Key::LControl }
	}
};

}
