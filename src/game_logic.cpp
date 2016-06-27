#include "game_logic.hpp"
#include "Bomb.hpp"
#include "Explosion.hpp"
#include "Controllable.hpp"
#include "Player.hpp"

using EntityList = std::list<Game::Entity*>;

void Game::Logic::bombExplosionLogic(Game::Entity *e, Game::LevelManager& lm,
		EntityList& tbspawned, EntityList& tbkilled)
{
	auto bomb = dynamic_cast<Game::Bomb*>(e);
	if (bomb == nullptr) return;

	if (bomb->isFuseOver()) {
		auto killable = bomb->get<Game::Killable>();
		if (killable->isKilled()) return;
		killable->kill();
		lm.rmBomb(bomb);
		tbspawned.push_back((new Game::Explosion(bomb->getPosition(),
				bomb->getRadius(), &bomb->getSourcePlayer()))->propagate(lm));
		tbkilled.push_back(bomb);
	}
}

void Game::Logic::bombDeployLogic(Game::Entity *e, Game::LevelManager &lm,
		EntityList& tbspawned, EntityList& tbkilled)
{
	if (!lm.isPlayer(e)) return;
	auto player = static_cast<Game::Player*>(e);

	if (player->get<Game::Controllable>()->hasFocus() 
		&& player->isAligned() 
		&& sf::Keyboard::isKeyPressed(
			Game::Controls::players[player->getInfo().id-1][Game::Controls::CTRL_BOMB])
		&& !lm.isBombAt(player->getPosition()))
	{
		const auto pw = player->getInfo().powers;
		tbspawned.push_back(new Game::Bomb(player->getPosition(), *player, pw.bombFuseTime, pw.bombRadius));
	}
}

std::vector<Game::Logic::GameLogicFunc> Game::Logic::functions = {
	bombDeployLogic,
	bombExplosionLogic
};
