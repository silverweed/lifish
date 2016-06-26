#include "game_logic.hpp"
#include "Bomb.hpp"
#include "Explosion.hpp"
#include "Controllable.hpp"
#include "Player.hpp"

void Game::Logic::bombExplosionLogic(Game::Entity *e, Game::LevelManager& lm) {
	auto bomb = dynamic_cast<Game::Bomb*>(e);
	if (bomb == nullptr) return;

	if (bomb->isFuseOver()) {
		bomb->get<Game::Killable>()->kill();
		lm.spawn(new Game::Explosion(bomb->getPosition(), bomb->getRadius(), &bomb->getSourcePlayer()));
	}
}

void Game::Logic::bombDeployLogic(Game::Entity *e, Game::LevelManager &lm) {
	if (!lm.isPlayer(e)) return;
	auto player = static_cast<Game::Player*>(e);

	if (player->get<Game::Controllable>()->hasFocus() 
		&& player->isAligned() 
		&& sf::Keyboard::isKeyPressed(
			Game::Controls::players[player->getInfo().id-1][Game::Controls::CTRL_BOMB]))
	{
		const auto pw = player->getInfo().powers;
		lm.spawn(new Game::Bomb(player->getPosition(), *player, pw.bombFuseTime, pw.bombRadius));
	}
}

std::vector<Game::Logic::GameLogicFunc> Game::Logic::functions = {
	bombDeployLogic,
	bombExplosionLogic
};
