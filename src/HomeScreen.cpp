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
	 * Exit game
	 * -------
	 * FOOTER
	 */

	// TODO: Header
	// TODO: scroll text

	const auto font = Game::getAsset("fonts", Game::Fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);

	auto size = 24;
	auto text = new Game::ShadedText(font, "Start Game", sf::Vector2f(0, 0));

	text->setCharacterSize(size);
	auto pos = Game::center(text->getGlobalBounds(), win_bounds);
	text->setPosition(pos);
	texts["start"] = text;

	text = new Game::ShadedText(font, "Load Game", pos);
	text->setCharacterSize(size);
	text->setPosition(sf::Vector2f(Game::centerX(text->getGlobalBounds(), win_bounds), pos.y + 2*size));
	texts["load"] = text;

	text = new Game::ShadedText(font, "Preferences", pos);
	text->setCharacterSize(size);
	text->setPosition(sf::Vector2f(Game::centerX(text->getGlobalBounds(), win_bounds), pos.y + 4*size));
	texts["preferences"] = text;

	text = new Game::ShadedText(font, "Exit Game", pos);
	text->setCharacterSize(size);
	text->setPosition(sf::Vector2f(Game::centerX(text->getGlobalBounds(), win_bounds), pos.y + 6*size));
	texts["exit"] = text;

	// Footer
	auto txt = new sf::Text;
	txt->setFont(*Game::cache.loadFont(font));
	txt->setCharacterSize(10);
	txt->setString("lifish v." VERSION " rev." COMMIT );
	txt->setPosition(5, win_bounds.height - 18);
	elements.push_back(txt);
	
}
