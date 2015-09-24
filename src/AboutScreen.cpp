#include "AboutScreen.hpp"
#include "utils.hpp"

using Game::AboutScreen;

AboutScreen::AboutScreen() : Screen() {
	/* Layout:
	 * 
	 * ??? (TODO)
	 * ---------
	 * Exit
	 */

	const auto fontname = Game::getAsset("fonts", Game::Fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);

	const auto size = 24;
	const float spacing = 1.8;

	auto text = new Game::ShadedText(fontname, "Work in Progress", sf::Vector2f(0, 0));
	text->setCharacterSize(size);
	auto pos = Game::center(text->getGlobalBounds(), win_bounds) - sf::Vector2f(0, 4*spacing*size);
	text->setPosition(pos);
	elements.push_back(text);

	const sf::Font& font = *Game::cache.loadFont(fontname);
	auto txt = new sf::Text("Lifish - (c) 2015 Giacomo Parolini", font, 14);
	txt->setPosition(sf::Vector2f(Game::centerX(txt->getGlobalBounds(), win_bounds), pos.y + 2*spacing*size));
	elements.push_back(txt);

	txt = new sf::Text("Lifish is free software, licensed under", font, 14);
	txt->setPosition(Game::centerX(txt->getGlobalBounds(), win_bounds), pos.y + 3*spacing*size);
	elements.push_back(txt);

	txt = new sf::Text("the GNU GPL v.3 or later.", font, 14);
	txt->setPosition(Game::centerX(txt->getGlobalBounds(), win_bounds), pos.y + 4*spacing*size);
	elements.push_back(txt);

	txt = new sf::Text("This is a pre-release! A lot of content", font, 14);
	txt->setPosition(Game::centerX(txt->getGlobalBounds(), win_bounds), pos.y + 5*spacing*size);
	elements.push_back(txt);

	txt = new sf::Text("is missing and there may be bugs.", font, 14);
	txt->setPosition(Game::centerX(txt->getGlobalBounds(), win_bounds), pos.y + 6*spacing*size);
	elements.push_back(txt);

	text = new Game::ShadedText(fontname, "Exit", pos);
	text->setCharacterSize(size);
	auto bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(Game::center(bounds, win_bounds).x, win_bounds.height - 2 * bounds.height));
	texts["exit"] = text;
}
