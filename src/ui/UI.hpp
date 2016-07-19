#pragma once

#include <SFML/Graphics.hpp>
#include "WithOrigin.hpp"
#include "ScreenHandler.hpp"

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

	/** Loads all screens from `scrNames` into `screenHandler`. Will ignore already-loaded screens. */
	void load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames);

	bool isActive() const { return active; }
	void setActive(bool b) { active = b; }

	/** UI-specific event loop, to be called when UI is active (instead of the main event loop) */
	void handleEvents(sf::Window& window);

	void setOrigin(const sf::Vector2f& pos) override {
		Game::WithOrigin::setOrigin(pos);
		screenHandler.setOrigin(pos);
	}

	void update() {
		if (active)
			screenHandler.update();
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (active)
			target.draw(screenHandler, states);
	}
};

} // end namespace UI

} // end namespace Game
