#include "PauseScreen.hpp"
#include "utils.hpp"

using Game::PauseScreen;

PauseScreen::PauseScreen() : Screen() {
	/* Layout:
	 * 
	 * Save Game
	 * Preferences
	 * ----
	 * Exit
	 */
	const auto font = Game::getAsset("fonts", Game::Fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);

	const auto size = 24;
	const float spacing = 1.8;

	auto text = new Game::ShadedText(font, "Save Game", sf::Vector2f(0, 0));
	text->setCharacterSize(size);
	auto pos = Game::center(text->getGlobalBounds(), win_bounds);
	text->setPosition(pos);
	texts["save"] = text;

	text = new Game::ShadedText(font, "Preferences", pos);
	text->setCharacterSize(size);
	text->setPosition(sf::Vector2f(Game::centerX(text->getGlobalBounds(), win_bounds), pos.y + spacing*size));
	texts["preferences"] = text;

	text = new Game::ShadedText(font, "Resume", pos);
	text->setCharacterSize(size);
	text->setPosition(sf::Vector2f(Game::centerX(text->getGlobalBounds(), win_bounds), pos.y + 3*spacing*size));
	texts["exit"] = text;
}
