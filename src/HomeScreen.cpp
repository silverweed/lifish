#include "HomeScreen.hpp"
#include "utils.hpp"

using Game::HomeScreen;

HomeScreen::HomeScreen() : Screen() {
	/* Layout:
	 * 
	 * HEADER
	 * -------
	 * SCROLL TEXT
	 * -------
	 * Start game
	 * Load game
	 * Preferences
	 * About
	 * Exit game
	 * -------
	 * FOOTER
	 */

	// TODO: Header
	// TODO: scroll text

	const auto font = Game::getAsset("fonts", Game::Fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);

	const auto size = 24;
	const float spacing = 1.8;

	auto text = new Game::ShadedText(font, "Start Game", sf::Vector2f(0, 0));
	text->setCharacterSize(size);
	auto pos = Game::center(text->getGlobalBounds(), win_bounds);
	text->setPosition(pos);
	texts["start"] = text;

	text = new Game::ShadedText(font, "Load Game", pos);
	text->setCharacterSize(size);
	text->setPosition(sf::Vector2f(Game::centerX(text->getGlobalBounds(), win_bounds), pos.y + spacing*size));
	texts["load"] = text;

	text = new Game::ShadedText(font, "Preferences", pos);
	text->setCharacterSize(size);
	text->setPosition(sf::Vector2f(Game::centerX(text->getGlobalBounds(), win_bounds), pos.y + 2*spacing*size));
	texts["preferences"] = text;

	text = new Game::ShadedText(font, "About", pos);
	text->setCharacterSize(size);
	text->setPosition(sf::Vector2f(Game::centerX(text->getGlobalBounds(), win_bounds), pos.y + 3*spacing*size));
	texts["about"] = text;

	text = new Game::ShadedText(font, "Exit Game", pos);
	text->setCharacterSize(size);
	text->setPosition(sf::Vector2f(Game::centerX(text->getGlobalBounds(), win_bounds), pos.y + 4*spacing*size));
	texts["exit"] = text;

	// Footer
	auto txt = new sf::Text("lifish v." VERSION " rev." COMMIT, Game::cache.loadFont(font), 10);
	txt->setPosition(5, win_bounds.height - 18);
	elements.push_back(txt);
}
