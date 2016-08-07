#include "game_logic.hpp"
#include "Bomb.hpp"
#include "Explosion.hpp"
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
		lm.rmBomb(bomb);
		tbkilled.push_back(bomb);
	}
}

void Game::Logic::bombDeployLogic(Game::Entity *e, Game::LevelManager& lm,
		EntityList& tbspawned, EntityList&)
{
	if (!lm.isPlayer(e)) return;
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
		const auto bonus_type = Game::bonusTypeDistribution(Game::rng);
		if (bonus_type < Game::Bonus::N_BONUS_TYPES) {
			tbspawned.push_back(new Game::Bonus(e->getPosition(),
						static_cast<Game::Bonus::Type>(bonus_type)));
		}
	}
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

void Game::Logic::enemiesShootLogic(Game::Entity *e, Game::LevelManager&,
		EntityList& tbspawned, EntityList&)
{
	auto enemy = dynamic_cast<const Game::Enemy*>(e);
	if (enemy == nullptr) return;

	auto bullet = enemy->checkShoot();
	if (bullet != nullptr) {
		Game::cache.playSound(bullet->get<Game::Sounded>()->getSoundFile(Game::Sounds::SHOT));
		tbspawned.push_back(bullet);
	}
}

void Game::Logic::bulletsHitLogic(Game::Entity *e, Game::LevelManager&,
		EntityList&, EntityList&)
{
	auto bullet = dynamic_cast<Game::Bullet*>(e);
	if (bullet == nullptr) return;

	if (bullet->get<Game::Killable>()->isKilled() && !bullet->hasDealtDamage()) {
		Game::cache.playSound(bullet->get<Game::Sounded>()->getSoundFile(Game::Sounds::DEATH));
		bullet->dealDamage(); // don't process this bullet again

		auto hit = bullet->getEntityHit();
		if (hit == nullptr) return;

		auto lifed = hit->get<Game::Lifed>();
		if (lifed == nullptr) return;

		auto bns = hit->get<Game::Bonusable>();
		if (bns == nullptr || !bns->hasBonus(Game::Bonus::SHIELD)) {
			lifed->decLife(bullet->getDamage());
			auto snd = hit->get<Game::Sounded>();
			if (snd != nullptr)
				Game::cache.playSound(snd->getSoundFile(Game::Sounds::HURT));
			if (bns != nullptr)
				bns->giveBonus(Game::Bonus::SHIELD, Game::Conf::DAMAGE_SHIELD_TIME);
		}
	}
}

void Game::Logic::explosionDamageLogic(Game::Entity *e, Game::LevelManager &lm,	EntityList&, EntityList&)
{
	auto expl = dynamic_cast<Game::Explosion*>(e);
	if (expl == nullptr) return;
	expl->checkHit(lm);
}

std::vector<Game::Logic::GameLogicFunc> Game::Logic::functions = {
	bombDeployLogic,
	bombExplosionLogic,
	bonusDropLogic,
	scoredKillablesLogic,
	explosionDamageLogic,
	enemiesShootLogic,
	bulletsHitLogic
};
