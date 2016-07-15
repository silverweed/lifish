#include "Screen.hpp"
#include "ScreenBuilder.hpp"
#include <iostream>

using Game::UI::Screen;

Screen::Screen(const std::string& layoutFileName) {
	Game::UI::ScreenBuilder::build(*this, layoutFileName);	
}

void Screen::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(bgSprite, states);
	for (const auto& elem : nonInteractables)
		target.draw(*elem, states);
	for (const auto& text : texts)
		target.draw(*text.second, states);
	for (const auto& img : images)
		target.draw(*img.second, states);
}
