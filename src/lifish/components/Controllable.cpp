#include "Controllable.hpp"
#include <exception>

using Game::Controllable;

Controllable::Controllable(Game::Entity *const owner,
		const std::array<sf::Keyboard::Key, Game::Controls::CONTROLS_NUM>& controls)
	: Game::Component(owner)
	, controls(controls)
{
	moving = owner->get<Game::AxisMoving>();
	if (moving == nullptr)
		throw std::invalid_argument("Entity passed to Controllable has no AxisMoving component!");
}

void Controllable::update() {
	Game::Component::update();
	if (window == nullptr)
		throw std::logic_error("window is null in Controllable::update()!");

	Game::Direction dir(Game::Direction::NONE);

	if (window->hasFocus()) {
		if (joystickUsed >= 0) {
			const auto horizontal = sf::Joystick::getAxisPosition(joystickUsed, sf::Joystick::X),
				   vertical = sf::Joystick::getAxisPosition(joystickUsed, sf::Joystick::Y);
			if (vertical < -Game::JOYSTICK_INPUT_THRESHOLD) 
				dir = Game::Direction::UP;
			else if (vertical > Game::JOYSTICK_INPUT_THRESHOLD)
				dir = Game::Direction::DOWN;
			else if (horizontal < -Game::JOYSTICK_INPUT_THRESHOLD)
				dir = Game::Direction::LEFT;
			else if (horizontal > Game::JOYSTICK_INPUT_THRESHOLD)
				dir = Game::Direction::RIGHT;
		} else {
			if (sf::Keyboard::isKeyPressed(controls[Game::Controls::CTRL_UP]))
				dir = Game::Direction::UP;
			else if (sf::Keyboard::isKeyPressed(controls[Game::Controls::CTRL_LEFT]))
				dir = Game::Direction::LEFT;
			else if (sf::Keyboard::isKeyPressed(controls[Game::Controls::CTRL_DOWN]))
				dir = Game::Direction::DOWN;
			else if (sf::Keyboard::isKeyPressed(controls[Game::Controls::CTRL_RIGHT]))
				dir = Game::Direction::RIGHT;
		}
	}

	if (owner->isAligned())
		moving->setDirection(dir);
}
