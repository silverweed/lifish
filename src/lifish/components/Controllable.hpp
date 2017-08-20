#pragma once

#include <array>
#include <SFML/Window.hpp>
#include <SFML/System/Time.hpp>
#include "Component.hpp"
#include "controls.hpp"

namespace lif {

class AxisMoving;

/** Controllable makes an AxisMoving Entity move taking input from the user. */
class Controllable : public lif::Component {
	const sf::Window *window = nullptr;
	/** Reference to an external array telling us how to map keys to controls */
	const std::array<sf::Keyboard::Key, lif::controls::CONTROLS_NUM>& controls;

	bool usedBomb = false;

	/** Reference to an external variable telling us if we're using a joystick */
	short& joystickUsed;
	/** Reference to an external variable containing the button number for the joystick bomb command */
	unsigned& joystickBombKey;

	sf::Clock disableClock;
	sf::Time disableTime = sf::Time::Zero;

	lif::AxisMoving *moving = nullptr;

	void _queueBombCommand();
public:
	explicit Controllable(lif::Entity& owner,
			const std::array<sf::Keyboard::Key, lif::controls::CONTROLS_NUM>& controls,
			short& joystickUsed,
			unsigned& joystickBombKey);

	lif::Entity* init() override;
	void update() override;

	void setWindow(const sf::Window& w) { window = &w; }

	bool hasFocus() const { return window != nullptr && window->hasFocus(); }

	void disableFor(const sf::Time& time) { disableTime = time; disableClock.restart(); }

	bool hasQueuedBombCommand() const;
};

}
