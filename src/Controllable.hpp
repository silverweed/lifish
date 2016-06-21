#pragma once

#include <unordered_map>
#include "Component.hpp"
#include "AxisMoving.hpp"
#include "controls.hpp"

namespace Game {

/** Controllable makes an AxisMoving Entity move taking input from the user. */
class Controllable : public Game::Component {
	const sf::Window *window = nullptr;
	const std::array<sf::Keyboard::Key, Game::Controls::CONTROLS_NUM>& controls;
	short joystickUsed = -1;
	Game::AxisMoving *moving = nullptr;

public:
	explicit Controllable(Game::Entity *const owner, 
			const std::array<sf::Keyboard::Key, Game::Controls::CONTROLS_NUM>& controls);

	void update() override;

	void setWindow(const sf::Window *w) { window = w; }
	void setJoystickUsed(short idx) { joystickUsed = idx; }

	bool hasFocus() const { return window && window->hasFocus(); }
};

}
