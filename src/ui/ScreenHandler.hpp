#pragma once

#include <string>
#include <unordered_map>
#include <initializer_list>
#include <SFML/Graphics.hpp>
#include "WithOrigin.hpp"
#include "Screen.hpp"

namespace Game {

namespace UI {

class ScreenHandler : public sf::Drawable, public Game::WithOrigin {
	std::unordered_map<std::string, std::unique_ptr<Game::UI::Screen>> screens;
	Game::UI::Screen *curScreen = nullptr;

public:
	ScreenHandler() {}
	ScreenHandler(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames);

	void load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames);
	
	void update() {
		if (curScreen != nullptr)
			curScreen->update();
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (curScreen != nullptr)
			target.draw(*curScreen, states);
	}

	void fireClick();
	void setCurrent(const std::string& name);
	void setCurrentToParent();
};

} // end namespace UI

} // end namespace Game
