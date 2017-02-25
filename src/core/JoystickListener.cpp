#include "JoystickListener.hpp"
#include "core.hpp"
#include "utils.hpp"

using lif::joystick::JoystickListener;

constexpr float JOYSTICK_EVT_ACCEPTANCE_VALUE = 80;
const sf::Time MAX_EVT_TIME = sf::seconds(0.5);

JoystickListener::JoystickListener(unsigned jid) : id(jid) {}

void JoystickListener::update() {
	if (!sf::Joystick::isConnected(id))
		return;

	evtRegistered.clear();
	
	_listen(sf::Joystick::X);
	_listen(sf::Joystick::Y);
	_listen(sf::Joystick::U);
	_listen(sf::Joystick::V);
	//_listen(sf::Joystick::Z);
	//_listen(sf::Joystick::R);
}

std::pair<JoystickListener::Axis, JoystickListener::Axis> JoystickListener::_getPair(sf::Joystick::Axis axis) const {
	using A = JoystickListener::Axis;
	switch (axis) {
	case sf::Joystick::X:
		return std::make_pair(A::L_LEFT, A::L_RIGHT);
	case sf::Joystick::Y:
		return std::make_pair(A::L_UP, A::L_DOWN);
	case sf::Joystick::U:
		return std::make_pair(A::R_LEFT, A::R_RIGHT);
	case sf::Joystick::V:
		return std::make_pair(A::R_UP, A::R_DOWN);
	default:
		break;
	}
	// FIXME
	return std::make_pair(A::LTRIGGER, A::RTRIGGER);
}

void JoystickListener::_listen(sf::Joystick::Axis ax) {
	if (!sf::Joystick::hasAxis(id, ax))
		return;

	const float val = sf::Joystick::getAxisPosition(id, ax);
	const float aval = lif::abs(val);
	const auto pair = _getPair(ax);
	const auto axis = val < 0 ? pair.first : pair.second;
	auto& started = evtStarted[axis];
	auto& ascending = evtAscending[axis];

	if (!started) {
		if (aval > lif::JOYSTICK_INPUT_THRESHOLD) {
			started = true;
			ascending = true;
		}
	} else {
		if (ascending && aval > JOYSTICK_EVT_ACCEPTANCE_VALUE) {
			ascending = false;
		} else if (!ascending && aval < lif::JOYSTICK_INPUT_THRESHOLD) {
			evtRegistered[axis] = true;
			started = false;
		}
	}
}

bool JoystickListener::evtMoved(JoystickListener::Axis a) const {
	const auto moved = evtRegistered.find(a);
	if (moved == evtRegistered.end())
		return false;
	return moved->second;
}

bool JoystickListener::isAnyEvtMoved() const {
	for (auto it = evtRegistered.begin(); it != evtRegistered.end(); ++it)
		if (it->second) return true;
	return false;
}
