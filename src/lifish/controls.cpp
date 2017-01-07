#include "controls.hpp"

std::array<sf::Keyboard::Key, lif::controls::CONTROLS_NUM> lif::controls::players[] =
{
	{{
		/* Control::UP,   */ sf::Keyboard::Key::Up,
		/* Control::DOWN, */ sf::Keyboard::Key::Down,
		/* Control::LEFT, */ sf::Keyboard::Key::Left,
		/* Control::RIGHT,*/ sf::Keyboard::Key::Right,
		/* Control::BOMB, */ sf::Keyboard::Key::RControl
	}},
	{{
		/* Control::UP,    */ sf::Keyboard::Key::W,
		/* Control::DOWN,  */ sf::Keyboard::Key::S,
		/* Control::LEFT,  */ sf::Keyboard::Key::A,
		/* Control::RIGHT, */ sf::Keyboard::Key::D,
		/* Control::BOMB,  */ sf::Keyboard::Key::LControl
	}}
};

std::array<unsigned int, lif::MAX_PLAYERS> lif::controls::joystickBombKey = {{ 0, 0 }};
std::array<short, lif::MAX_PLAYERS> lif::controls::useJoystick;
