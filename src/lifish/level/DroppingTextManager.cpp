#include "DroppingTextManager.hpp"
#include "DroppingSprite.hpp"
#include "EntityGroup.hpp"
#include "core.hpp"
#include <SFML/System.hpp>

using Game::DroppingTextManager;

DroppingTextManager::DroppingTextManager()
	: gameOverText(new Game::DroppingSprite(
				Game::getAsset("test", "gameover.png"), sf::Vector2i(311, 59)))
	, extraGameText(new Game::DroppingSprite(
				Game::getAsset("test", "extragame.png"), sf::Vector2i(223, 156), 250.f))
	, hurryUpText(new Game::DroppingSprite(
				Game::getAsset("test", "hurryup.png"), sf::Vector2i(161, 30), 300.f))
{}

void DroppingTextManager::subscribe(Game::EntityGroup& group) {
	group.add(gameOverText);
	group.add(extraGameText);
	group.add(hurryUpText);
}

void DroppingTextManager::trigger(DroppingTextManager::Text text) {
	switch (text) {
		using T = DroppingTextManager::Text;
	case T::GAME_OVER:
		gameOverText->play();
		break;
	case T::EXTRA_GAME:
		extraGameText->play();
		break;
	case T::HURRY_UP:
		hurryUpText->play();
		break;
	}
}