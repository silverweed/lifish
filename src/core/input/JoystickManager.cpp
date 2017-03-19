#include "JoystickManager.hpp"

using lif::joystick::JoystickManager;

JoystickManager::JoystickManager()
	: listeners({{
		JoystickListener(0),
		JoystickListener(1),
		JoystickListener(2),
		JoystickListener(3),
		JoystickListener(4),
		JoystickListener(5),
		JoystickListener(6),
		JoystickListener(7)
	}})
{}

void JoystickManager::update() {
	sf::Joystick::update();

	for (auto& l : listeners)
		l.update();
}

int JoystickManager::isAnyEvtMoved() const {
	for (unsigned i = 0; i < listeners.size(); ++i) {
		if (sf::Joystick::isConnected(i) && listeners[i].isAnyEvtMoved())
			return i;
	}
	return -1;
}

bool JoystickManager::handleEvent(sf::Window&, sf::Event) {
	return false;
}
