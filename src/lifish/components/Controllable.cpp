#include "Controllable.hpp"
#include "AxisMoving.hpp"
#include <exception>

using lif::Controllable;

Controllable::Controllable(lif::Entity& owner,
		const std::array<sf::Keyboard::Key, lif::controls::CONTROLS_NUM>& controls,
		short& joystickUsed)
	: lif::Component(owner)
	, controls(controls)
	, joystickUsed(joystickUsed)
{
	_declComponent<Controllable>();
}

lif::Entity* Controllable::init() {
	moving = owner.get<lif::AxisMoving>();
	if (moving == nullptr)
		throw std::invalid_argument("Entity passed to Controllable has no AxisMoving component!");
	return this;
}

void Controllable::update() {
	lif::Component::update();
	if (window == nullptr)
		throw std::logic_error("window is null in Controllable::update()!");

	lif::Direction dir(lif::Direction::NONE);

	if (window->hasFocus()) {
		if (joystickUsed >= 0) {
			const auto horizontal = sf::Joystick::getAxisPosition(joystickUsed, sf::Joystick::X),
				   vertical = sf::Joystick::getAxisPosition(joystickUsed, sf::Joystick::Y);
			if (vertical < -lif::JOYSTICK_INPUT_THRESHOLD) 
				dir = lif::Direction::UP;
			else if (vertical > lif::JOYSTICK_INPUT_THRESHOLD)
				dir = lif::Direction::DOWN;
			else if (horizontal < -lif::JOYSTICK_INPUT_THRESHOLD)
				dir = lif::Direction::LEFT;
			else if (horizontal > lif::JOYSTICK_INPUT_THRESHOLD)
				dir = lif::Direction::RIGHT;
		}
		if (dir == lif::Direction::NONE) {
			if (sf::Keyboard::isKeyPressed(controls[lif::controls::CTRL_UP]))
				dir = lif::Direction::UP;
			else if (sf::Keyboard::isKeyPressed(controls[lif::controls::CTRL_LEFT]))
				dir = lif::Direction::LEFT;
			else if (sf::Keyboard::isKeyPressed(controls[lif::controls::CTRL_DOWN]))
				dir = lif::Direction::DOWN;
			else if (sf::Keyboard::isKeyPressed(controls[lif::controls::CTRL_RIGHT]))
				dir = lif::Direction::RIGHT;
		}
	}

	if (owner.isAligned())
		moving->setDirection(dir);
}
