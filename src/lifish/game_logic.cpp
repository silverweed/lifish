#include "game_logic.hpp"
#include "Bomb.hpp"
#include "Explosion.hpp"
#include "GameCache.hpp"
#include "Spawning.hpp"
#include "Killable.hpp"
#include "Boss.hpp"
#include "LevelManager.hpp"
#include "Grabbable.hpp"
#include "AutoShooting.hpp"
#include "Bullet.hpp"
#include "Controllable.hpp"
#include "Sounded.hpp"
#include "Lifed.hpp"
#include "Bonusable.hpp"
#include "Enemy.hpp"
#include "Options.hpp"
#include "Scored.hpp"
#include "AI.hpp"
#include "Points.hpp"
#include "Player.hpp"
#include "Bonus.hpp"
#include "Letter.hpp"
#include "BreakableWall.hpp"
#include "ShootingPoint.hpp"

using EntityList = std::vector<lif::Entity*>;

void lif::game_logic::bombDeployLogic(lif::Entity *e, lif::BaseLevelManager& blm, EntityList& tbspawned) {
	auto& lm = static_cast<lif::LevelManager&>(blm);
	if (!lm.isPlayer(*e)) return;
	auto player = static_cast<lif::Player*>(e);

	const auto pinfo = player->getInfo();
	if (player->get<lif::Controllable>()->hasFocus() 
		&& player->isAligned() 
		&& ((lif::controls::useJoystick[pinfo.id-1] >= 0 
			&& sf::Joystick::isButtonPressed(
				lif::controls::useJoystick[pinfo.id-1], 
				lif::controls::joystickBombKey[pinfo.id-1]))
			|| sf::Keyboard::isKeyPressed(
				lif::controls::players[pinfo.id-1][lif::controls::CTRL_BOMB]))
		&& lm.bombsDeployedBy(pinfo.id) < pinfo.powers.maxBombs
		&& lm.canDeployBombAt(lif::tile(player->getPosition())))
	{
		auto bomb = new lif::Bomb(lif::aligned(player->getPosition()), 
					player, pinfo.powers.bombFuseTime, pinfo.powers.bombRadius,
					pinfo.powers.incendiaryBomb);
		lif::cache.playSound(bomb->get<lif::Sounded>()->getSoundFile("fuse"));
		tbspawned.push_back(bomb);
	}
}

void lif::game_logic::spawningLogic(lif::Entity *e, lif::BaseLevelManager& blm, EntityList& tbspawned) {
	auto& lm = static_cast<lif::LevelManager&>(blm);
	for (auto spawning : e->getAll<lif::Spawning>()) {
		while (spawning->shouldSpawn()) {
			auto spawned = spawning->spawn().release();
			if (spawned != nullptr) {
				if (auto expl = dynamic_cast<lif::Explosion*>(spawned))
					expl->propagate(lm);
				tbspawned.push_back(spawned);	
			}
		}
	}
}

void lif::game_logic::scoredKillablesLogic(lif::Entity *e, lif::BaseLevelManager& blm, EntityList& tbspawned) {
	auto scored = e->get<lif::Scored>();
	if (scored == nullptr || scored->hasGivenPoints()) return;

	auto& lm = static_cast<lif::LevelManager&>(blm);
	
	auto klb = e->get<lif::Killable>();
	if (klb != nullptr && klb->isKilled()) {
		// Special behaviour for bosses
		const bool is_boss = dynamic_cast<const lif::Boss*>(&klb->getOwner()) != nullptr;
		if (is_boss && klb->isKillInProgress()) return;

		// Give and spawn points
		auto target = scored->getTarget();
		if (target < 0) {
			lm.addScoreToAll(scored->getPointsGiven());
			scored->givePoints();
		} else {
			lm.addScore(target, scored->givePoints());
		}
		auto points = is_boss
			? new lif::Points(e->getPosition(), scored->getPointsGiven(), sf::Color::Magenta, 30)
			: new lif::Points(e->getPosition(), scored->getPointsGiven());
		sf::Vector2f bounds_size(lif::TILE_SIZE, lif::TILE_SIZE);
		const auto cld = e->get<lif::Collider>();
		if (cld != nullptr)
			bounds_size = sf::Vector2f(cld->getSize());
		points->setPosition(sf::Vector2f(
				lif::centerX(points->getGlobalBounds(), sf::FloatRect(e->getPosition(), bounds_size)),
				points->getPosition().y));
		tbspawned.push_back(points);
	}
}

void lif::game_logic::bonusGrabLogic(lif::Entity *e, lif::BaseLevelManager& blm, EntityList&) {
	auto bonus = dynamic_cast<lif::Bonus*>(e);
	if (bonus == nullptr) return;
	
	auto grb = bonus->get<lif::Grabbable>();
	if (grb->isGrabbed()) return;
	
	auto player = grb->getGrabbingEntity();
	if (player == nullptr) return;
	
	lif::triggerBonus(static_cast<lif::LevelManager&>(blm), bonus->getType(), *static_cast<lif::Player*>(player));
	grb->grab();
}

std::vector<lif::BaseLevelManager::GameLogicFunc> lif::game_logic::functions = {
	bombDeployLogic,
	bonusGrabLogic,
	scoredKillablesLogic,
	spawningLogic
};
