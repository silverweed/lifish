#pragma once

#include <array>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

namespace lif {

namespace joystick {

/**
 * This class is used to intercept events like "axis up", "axis left", etc. for joysticks (since SFML does not
 * provide the equivalent of `isButtonPressed` for such events).
 * Usage:
 * 	JoystickListener jl(n); // n is the joystick id
 * 	while (...) {
 * 		jl.update();
 * 		if (jl.evtMoved(JoystickListener::Axis::L_RIGHT)) { ... }
 * 	}
 */
class JoystickListener : private sf::NonCopyable {
public:
	enum class Axis { L_UP, L_DOWN, L_LEFT, L_RIGHT, R_UP, R_DOWN, R_LEFT, R_RIGHT, N_AXES };

private:
	const unsigned id;

	/** Whether the event associated with each axis was started or not */
	std::array<bool, static_cast<int>(Axis::N_AXES)> evtStarted;
	/** Map { axis => b }, where `b` is true if the event associated with `axis` is currently in the "ascending"
	 *  phase (i.e. we have yet to register a |value| >= JOYSTICK_EVT_ACCEPTANCE_VALUE) for that possible event)
	 *  or false if descending.
	 *  Both these values are meaningless if `evtStarted[axis] == false`.
	 */
	std::array<bool, static_cast<int>(Axis::N_AXES)> evtAscending;
	std::array<bool, static_cast<int>(Axis::N_AXES)> evtRegistered;
	Axis latestEvt = Axis::N_AXES;

	void _listen(sf::Joystick::Axis axis);
	/** @return a pair of axes which are the "negative" and "positive" halves of the SFML Axis passed */
	std::pair<Axis, Axis> _getPair(sf::Joystick::Axis axis) const;
public:
	JoystickListener(unsigned joystickId);
	JoystickListener(JoystickListener&& jl);

	/** @return true if a "move `a`" event was registered. "Moving `a`" is defined as follows:
	 *  the value of sf::Joystick::getAxisPosition(a):
	 *    1. started with a |value| <= JOYSTICK_THRESHOLD
	 *    2. reached |value| >= JOYSTICK_EVT_ACCEPTANCE_VALUE
	 *    3. returned to a |value| <= JOYSTICK_THRESHOLD
	 *  The event remains `true` for a brief time after being registered.
	 */
	bool evtMoved(Axis a) const;
	/** @return true if any axis has event == true */
	bool isAnyEvtMoved() const;
	Axis getLatestEvt() const;

	void update();
};

}

}
