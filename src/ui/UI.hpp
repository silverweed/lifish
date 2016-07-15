#pragma once

#include <SFML/Graphics.hpp>
#include "../WithOrigin.hpp"

namespace Game {

namespace UI {

/** High-level interface to UI. It's a singleton class */
class UI final : public sf::Drawable, public Game::WithOrigin {
	Game::UI::ScreenHandler screenHandler;
	bool active = false;

	UI() {}
public:
	UI(const UI&) = delete;

	static UI& getInstance() {
		static UI instance;
		return instance;
	}

	bool isActive() const { return active; }
	void setActive(bool b) { active = b; }

	void handleEvent(sf::Window& event);

	void setOrigin(const sf::Vector2f& pos) override {
		Game::WithOrigin::setOrigin(pos);
		screenHandler.setOrigin(pos);
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(screenHandler, states);
	}
};

} // end namespace UI

} // end namespace Game
