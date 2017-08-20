#include "JoystickListener.hpp"

using lif::joystick::JoystickListener;
using JAxis = JoystickListener::Axis;

constexpr short JOYSTICK_EVT_ACCEPTANCE_VALUE = 80;
constexpr short JOYSTICK_INPUT_THRESHOLD = 50;

JoystickListener::JoystickListener(unsigned jid) : id(jid) {}

JoystickListener::JoystickListener(JoystickListener&& jl)
	: id(jl.id)
	, evtStarted(jl.evtStarted)
	, evtAscending(jl.evtAscending)
	, evtRegistered(jl.evtRegistered)
	, latestEvt(jl.latestEvt)
{}

void JoystickListener::update() {
	if (!sf::Joystick::isConnected(id))
		return;

	evtRegistered.fill(false);

	_listen(sf::Joystick::X);
	_listen(sf::Joystick::Y);
	_listen(sf::Joystick::U);
	_listen(sf::Joystick::V);
}

std::pair<JAxis, JAxis> JoystickListener::_getPair(sf::Joystick::Axis axis) const {
	switch (axis) {
	case sf::Joystick::X:
		return std::make_pair(JAxis::L_LEFT, JAxis::L_RIGHT);
	case sf::Joystick::Y:
		return std::make_pair(JAxis::L_UP, JAxis::L_DOWN);
	case sf::Joystick::U:
		return std::make_pair(JAxis::R_LEFT, JAxis::R_RIGHT);
	case sf::Joystick::V:
		return std::make_pair(JAxis::R_UP, JAxis::R_DOWN);
	default:
		break;
	}
	return std::pair<JAxis, JAxis>();
}

void JoystickListener::_listen(sf::Joystick::Axis ax) {
	if (!sf::Joystick::hasAxis(id, ax))
		return;

	const float val = sf::Joystick::getAxisPosition(id, ax);
	const float aval = val > 0 ? val : -val;
	const auto pair = _getPair(ax);
	const auto axis = val < 0 ? pair.first : pair.second;
	auto& started = evtStarted[static_cast<int>(axis)];
	auto& ascending = evtAscending[static_cast<int>(axis)];

	if (!started) {
		if (aval > JOYSTICK_INPUT_THRESHOLD) {
			started = true;
			ascending = true;
		}
	} else {
		if (ascending && aval > JOYSTICK_EVT_ACCEPTANCE_VALUE) {
			ascending = false;
		} else if (!ascending && aval < JOYSTICK_INPUT_THRESHOLD) {
			evtRegistered[static_cast<int>(axis)] = true;
			latestEvt = axis;
			started = false;
		}
	}
}

bool JoystickListener::evtMoved(JoystickListener::Axis a) const {
	return evtRegistered[static_cast<int>(a)];
}

bool JoystickListener::isAnyEvtMoved() const {
	return std::find(evtRegistered.begin(), evtRegistered.end(), true) != evtRegistered.end();
}

JoystickListener::Axis JoystickListener::getLatestEvt() const {
	return latestEvt;
}
