#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Window.hpp>
#include "WithOrigin.hpp"

namespace Game {

class WindowContext : public sf::Drawable, public Game::WithOrigin {
	bool active;

public:
	void setActive(bool b) { active = b; }	
	bool isActive() const { return active; }
	/** @return The active state of this Context after the change */
	bool toggleActive() { return active = !active; }

	virtual void update() = 0;
	virtual void handleEvents(sf::Window& window) = 0;
};

}
