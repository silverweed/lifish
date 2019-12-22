#include "Controllable.hpp"
#include "AxisMoving.hpp"
#include "input_utils.hpp"
#include <exception>

using lif::Controllable;

Controllable::Controllable(lif::Entity& owner,
		const std::array<sf::Keyboard::Key, lif::controls::CONTROLS_NUM>& controls,
		short& joystickUsed, unsigned& joystickBombKey)
	: lif::Component(owner)
	, controls(controls)
	, joystickUsed(joystickUsed)
	, joystickBombKey(joystickBombKey)
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

	if (disableTime > sf::Time::Zero) {
		if (disableClock.getElapsedTime() > disableTime)
			disableTime = sf::Time::Zero;
		else
			return;
	}

	auto dir = lif::Direction::NONE;

	usedBomb = false;

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

			if (sf::Joystick::isButtonPressed(joystickUsed, joystickBombKey))
				usedBomb = true;
		} else {
			if (dir == lif::Direction::NONE) {
				if (IS_KEY_PRESSED(controls[lif::controls::CTRL_UP]))
					dir = lif::Direction::UP;
				else if (IS_KEY_PRESSED(controls[lif::controls::CTRL_LEFT]))
					dir = lif::Direction::LEFT;
				else if (IS_KEY_PRESSED(controls[lif::controls::CTRL_DOWN]))
					dir = lif::Direction::DOWN;
				else if (IS_KEY_PRESSED(controls[lif::controls::CTRL_RIGHT]))
					dir = lif::Direction::RIGHT;
			}

			if (IS_KEY_PRESSED(controls[lif::controls::CTRL_BOMB]))
				usedBomb = true;
		}
	}

	if (owner.isAligned())// || dir == lif::oppositeDirection(moving->getDirection()))
		moving->setDirection(dir);
}

bool Controllable::hasQueuedBombCommand() const {
	return usedBomb;
}
