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
	const std::array<sf::Keyboard::Key, Game::Controls::CONTROLS_NUM>& controls;
	short joystickUsed = -1;
	Game::AxisMoving *moving = nullptr;
	bool active = true;

public:
	explicit Controllable(Game::Entity& owner, 
			const std::array<sf::Keyboard::Key, Game::Controls::CONTROLS_NUM>& controls);

	Game::Entity* init() override;
	void update() override;

	void setWindow(const sf::Window& w) { window = &w; }
	void setJoystickUsed(short idx) { joystickUsed = idx; }

	bool hasFocus() const { return window != nullptr && window->hasFocus(); }
	void setActive(bool a) { active = a; }
};

}
