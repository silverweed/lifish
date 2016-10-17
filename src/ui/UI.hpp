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
	sf::Vector2u size;

	UI() : screenHandler(*this) {}
public:
	UI(const UI&) = delete;

	static UI& getInstance() {
		static UI instance;
		return instance;
	}

	Game::UI::ScreenHandler& getScreenHandler() { return screenHandler; }

	/** Loads all screens from `scrNames` into `screenHandler`. Will ignore already-loaded screens. */
	void load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames);
	/** Adds a custom Screen to the screenHandler (used to manage dynamic screens). */
	void add(Game::UI::Screen *screen);

	bool isActive() const { return active; }
	void setActive(bool b) { active = b; }
	/** @return The active state of UI after the change */
	bool toggleActive() { return active = !active; }

	/** The size of the UI is used to construct any screen loaded via `load`.
	 *  This method should be called once immediately after loading the screens.
	 */
	void setSize(const sf::Vector2u& sz) { size = sz; }
	sf::Vector2u getSize() const { return size; }

	/** UI-specific event loop, to be called when UI is active (instead of the main event loop) */
	void handleEvents(sf::Window& window);

	void update();

	void setOrigin(const sf::Vector2f& pos) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

} // end namespace UI

} // end namespace Game
