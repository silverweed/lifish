#pragma once

#include <array>
#include <SFML/Window.hpp>
#include "Component.hpp"
#include "controls.hpp"

namespace lif {

class AxisMoving;

/** Controllable makes an AxisMoving Entity move taking input from the user. */
class Controllable : public lif::Component {
	const sf::Window *window = nullptr;
	/** Reference to an external array telling us how to map keys to controls */
	const std::array<sf::Keyboard::Key, lif::controls::CONTROLS_NUM>& controls;
	/** Reference to an external variable telling us if we're using a joystick */
	short& joystickUsed;	
	lif::AxisMoving *moving = nullptr;

public:
	explicit Controllable(lif::Entity& owner, 
			const std::array<sf::Keyboard::Key, lif::controls::CONTROLS_NUM>& controls,
			short& joystickUsed);

	lif::Entity* init() override;
	void update() override;

	void setWindow(const sf::Window& w) { window = &w; }

	bool hasFocus() const { return window != nullptr && window->hasFocus(); }
};

}
