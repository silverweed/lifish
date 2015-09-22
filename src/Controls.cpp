#include "Controls.hpp"

using Game::Control;

std::map<Control, sf::Keyboard::Key> Game::playerControls[] =
{
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

std::array<unsigned int, Game::MAX_PLAYERS> Game::joystickBombKey = {{ 0, 0 }};
