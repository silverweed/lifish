#pragma once

#include <array>
#include <SFML/Window.hpp>
#include "Component.hpp"
#include "controls.hpp"

namespace Game {

class AxisMoving;

/** Controllable makes an AxisMoving Entity move taking input from the user. */
class Controllable : public Game::Component {
	const sf::Window *window = nullptr;
	/** Reference to an external array telling us how to map keys to controls */
	const std::array<sf::Keyboard::Key, Game::Controls::CONTROLS_NUM>& controls;
	/** Reference to an external variable telling us if we're using a joystick */
	short& joystickUsed;	
	Game::AxisMoving *moving = nullptr;
	bool active = true;

public:
	explicit Controllable(Game::Entity& owner, 
			const std::array<sf::Keyboard::Key, Game::Controls::CONTROLS_NUM>& controls,
			short& joystickUsed);

	Game::Entity* init() override;
	void update() override;

	void setWindow(const sf::Window& w) { window = &w; }

	bool hasFocus() const { return window != nullptr && window->hasFocus(); }
	void setActive(bool a) { active = a; }
};

}
