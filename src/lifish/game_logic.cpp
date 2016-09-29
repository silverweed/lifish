#include "game_logic.hpp"
#include "Bomb.hpp"
#include "Explosion.hpp"
#include "GameCache.hpp"
#include "Killable.hpp"
#include "LevelManager.hpp"
#include "Grabbable.hpp"
#include "AutoShooting.hpp"
#include "Bullet.hpp"
#include "Controllable.hpp"
#include "Sounded.hpp"
#include "Lifed.hpp"
#include "Bonusable.hpp"
#include "Enemy.hpp"
#include "Scored.hpp"
#include "AI.hpp"
#include "Points.hpp"
#include "Player.hpp"
#include "Bonus.hpp"
#include "Letter.hpp"
#include "BreakableWall.hpp"

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
		auto expl = new Game::Explosion(bomb->getPosition(),
				bomb->getRadius(), &bomb->getSourcePlayer());
		Game::cache.playSound(expl->get<Game::Sounded>()->getSoundFile(Game::Sounds::DEATH));
		tbspawned.push_back(expl->propagate(lm));
		tbkilled.push_back(bomb);
	}
}

void Game::Logic::bombDeployLogic(Game::Entity *e, Game::LevelManager& lm,
		EntityList& tbspawned, EntityList&)
{
	if (!lm.isPlayer(*e)) return;
	auto player = static_cast<Game::Player*>(e);

	const auto pinfo = player->getInfo();
	if (player->get<Game::Controllable>()->hasFocus() 
		&& player->isAligned() 
		&& sf::Keyboard::isKeyPressed(
			Game::Controls::players[player->getInfo().id-1][Game::Controls::CTRL_BOMB])
		&& lm.bombsDeployedBy(pinfo.id) < pinfo.powers.maxBombs
		&& !lm.isBombAt(Game::tile(player->getPosition())))
	{
		auto bomb = new Game::Bomb(Game::aligned(player->getPosition()), 
					*player, pinfo.powers.bombFuseTime, pinfo.powers.bombRadius);
		Game::cache.playSound(bomb->get<Game::Sounded>()->getSoundFile(Game::Sounds::DEATH));
		tbspawned.push_back(bomb);
	}
}

void Game::Logic::bonusDropLogic(Game::Entity *e, Game::LevelManager&,
		EntityList& tbspawned, EntityList&)
{
	auto wall = dynamic_cast<Game::BreakableWall*>(e);
	if (wall == nullptr) return;

	auto klb = wall->get<Game::Killable>();
	if (klb->isKilled() && !klb->isKillInProgress()) {
		const auto bonus_type = Game::Conf::Bonus::distribution(Game::rng);
		if (bonus_type < Game::Conf::Bonus::N_BONUS_TYPES) {
			tbspawned.push_back(new Game::Bonus(e->getPosition(),
						static_cast<Game::BonusType>(bonus_type)));
		}
	}
}

void Game::Logic::lettersDropLogic(Game::Entity *e, Game::LevelManager&,
		EntityList& tbspawned, EntityList&)
{
	auto enemy = dynamic_cast<Game::Enemy*>(e);
	if (enemy == nullptr || !enemy->isMorphed()) return;

	auto klb = enemy->get<Game::Killable>();
	if (klb->isKilled() && !klb->isKillInProgress())
		tbspawned.push_back(new Game::Letter(e->getPosition(), Game::Letter::randomId()));
}

void Game::Logic::scoredKillablesLogic(Game::Entity *e, Game::LevelManager&,
		EntityList& tbspawned, EntityList&)
{
	auto scored = e->get<Game::Scored>();
	if (scored == nullptr || scored->hasGivenPoints()) return;
	
	auto klb = e->get<Game::Killable>();
	if (klb != nullptr && klb->isKilled()) {
		// Give and spawn points
		auto target = scored->getTarget();
		if (target < 0) {
			// give points to all players
			for (auto& s : Game::score)
				s += scored->getPointsGiven();
			scored->givePoints();
		} else {
			Game::score[target - 1] += scored->givePoints();
			tbspawned.push_back(new Game::Points(e->getPosition(), scored->getPointsGiven()));
		}
	}
}

void Game::Logic::shootLogic(Game::Entity *e, Game::LevelManager&,
		EntityList& tbspawned, EntityList&)
{
	auto shootings = e->getAllRecursive<AutoShooting>();
	for (auto shooting : shootings) {
		auto bullet = shooting->pollShot();
		while (bullet != nullptr) {
			Game::cache.playSound(bullet->get<Game::Sounded>()->getSoundFile(Game::Sounds::SHOT));
			tbspawned.push_back(bullet.release());
			bullet = shooting->pollShot();
		}
	}
}

void Game::Logic::bonusGrabLogic(Game::Entity *e, Game::LevelManager &lm, EntityList&, EntityList&) {
	auto bonus = dynamic_cast<Game::Bonus*>(e);
	if (bonus == nullptr) return;
	
	auto grb = bonus->get<Game::Grabbable>();
	if (grb->isGrabbed()) return;
	
	auto player = grb->getGrabbingEntity();
	if (player == nullptr) return;
	
	Game::triggerBonus(lm, bonus->getType(), *static_cast<Game::Player*>(player));
	grb->grab();
}

std::vector<Game::Logic::GameLogicFunc> Game::Logic::functions = {
	bombDeployLogic,
	bombExplosionLogic,
	bonusDropLogic,
	lettersDropLogic,
	scoredKillablesLogic,
	shootLogic,
	bonusGrabLogic
};
