#include "controls.hpp"

std::array<sf::Keyboard::Key, lif::controls::CONTROLS_NUM> lif::controls::players[] =
{
	{{
		/* Control::UP,   */ sf::Keyboard::Key::Up,
		/* Control::DOWN, */ sf::Keyboard::Key::Down,
		/* Control::LEFT, */ sf::Keyboard::Key::Left,
		/* Control::RIGHT,*/ sf::Keyboard::Key::Right,
#if defined(SFML_SYSTEM_MACOS)
		/* Control::BOMB, */ sf::Keyboard::Key::RSystem
#else
		/* Control::BOMB, */ sf::Keyboard::Key::RControl
#endif
	}},
	{{
		/* Control::UP,    */ sf::Keyboard::Key::W,
		/* Control::DOWN,  */ sf::Keyboard::Key::S,
		/* Control::LEFT,  */ sf::Keyboard::Key::A,
		/* Control::RIGHT, */ sf::Keyboard::Key::D,
#if defined(SFML_SYSTEM_MACOS)
		/* Control::BOMB,  */ sf::Keyboard::Key::LSystem
#else
		/* Control::BOMB,  */ sf::Keyboard::Key::LControl
#endif
	}}
};

std::array<unsigned, lif::MAX_PLAYERS> lif::controls::joystickBombKey = {{ 0, 0 }};
std::array<short, lif::MAX_PLAYERS> lif::controls::useJoystick;
