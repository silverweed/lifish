#pragma once

#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "../WithOrigin.hpp"

namespace Game {

namespace UI {

class ScreenHandler : public sf::Drawable, public Game::WithOrigin {
	std::unordered_map<std::string, Game::UI::Screen> screens;
	Game::Screen& curScreen;

public:
	explicit ScreenHandler() 
		: curScreen(Game::UI::Screen())
	{
		screens["_blank"] = curScreen;
	}
	
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(curScreen, states);
	}
};

} // end namespace UI

} // end namespace Game
