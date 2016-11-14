#pragma once

#include <memory>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "WindowContext.hpp"

namespace Game {

namespace UI {

class Screen;

/** High-level interface to UI. It's a singleton class */
class UI final : public Game::WindowContext {
	std::unordered_map<std::string, std::unique_ptr<Game::UI::Screen>> screens;
	Game::UI::Screen *curScreen = nullptr;
	sf::Vector2u size;

	UI();
public:
	UI(const UI&) = delete;

	static UI& getInstance() {
		static UI instance;
		return instance;
	}

	/** Loads all screens from `scrNames` into `screens`. Will ignore already-loaded screens. */
	void load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames);
	/** Adds a custom Screen to the screens (used to manage dynamic screens). */
	void add(Game::UI::Screen *screen);

	/** The size of the UI is used to construct any screen loaded via `load`.
	 *  This method should be called once immediately after loading the screens.
	 */
	void setSize(const sf::Vector2u& sz) { size = sz; }
	sf::Vector2u getSize() const { return size; }

	void setCurrent(const std::string& name, bool overrideParent = false);
	void setCurrentToParent();
	void fireClick();

	/** UI-specific event loop, to be called when UI is active (instead of the main event loop) */
	bool handleEvent(sf::Window& window, sf::Event evt) override;
	void update() override;
	void setOrigin(const sf::Vector2f& pos) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

} // end namespace UI

} // end namespace Game
