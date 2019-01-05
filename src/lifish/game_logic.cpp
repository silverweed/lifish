#include "game_logic.hpp"
#include "AI.hpp"
#include "AbsorbFX.hpp"
#include "Absorbable.hpp"
#include "AxisMoving.hpp"
#include "Bomb.hpp"
#include "Bonus.hpp"
#include "Bonusable.hpp"
#include "Boss.hpp"
#include "BreakableWall.hpp"
#include "Bullet.hpp"
#include "Controllable.hpp"
#include "Enemy.hpp"
#include "Explosion.hpp"
#include "GameCache.hpp"
#include "Grabbable.hpp"
#include "Killable.hpp"
#include "Letter.hpp"
#include "LevelManager.hpp"
#include "Lifed.hpp"
#include "Options.hpp"
#include "Player.hpp"
#include "Points.hpp"
#include "Scored.hpp"
#include "ShootingPoint.hpp"
#include "Sounded.hpp"
#include "Spawning.hpp"
#include "Time.hpp"
#include <array>

using EntityList = std::vector<lif::Entity*>;

void lif::game_logic::bombDeployLogic(lif::Entity *e, lif::BaseLevelManager& blm, EntityList& tbspawned) {
	// Used for throwable bombs (pretty ugly, consider refactoring)
	static std::array<sf::Time, lif::MAX_PLAYERS> latestBomb;

	auto& lm = static_cast<lif::LevelManager&>(blm);
	if (!lm.isPlayer(*e)) return;
	auto player = static_cast<lif::Player*>(e);

	const auto pinfo = player->getInfo();
	const auto controllable = player->get<lif::Controllable>();
	if (controllable->hasFocus()
		&& !blm.isInputDisabled()
		&& lm.canDeployBomb(*player)
		&& controllable->hasQueuedBombCommand())
	{
		if (pinfo.powers.throwableBomb) {
			const auto gameTime = lif::time.getGameTime();
			if (gameTime - latestBomb[pinfo.id - 1] < sf::seconds(0.5))
				return;

			latestBomb[pinfo.id - 1] = gameTime;

			if (lm.bombsDeployedBy(pinfo.id) > 0) {
				auto bomb = lm.getFirstValidBomb(pinfo.id);
				if (bomb == nullptr)
					throw std::logic_error("Player deployed some bomb, but none found?!");

				bomb->ignite();

				std::cerr << "1\n";
				return;

			}

		} else if (!lm.canDeployBombAt(lif::tile2(player->getPosition()))) {
			// doing this check only if !throwableBomb, else the player wouldn't be
			// able to detonate a bomb while on the same tile as it
			return;
		}

		auto bomb = new lif::Bomb(lif::aligned2(player->getPosition()),
					player, pinfo.powers.bombFuseTime, pinfo.powers.bombRadius,
					pinfo.powers.incendiaryBomb);
		lif::cache.playSound(bomb->get<lif::Sounded>()->getSoundFile("fuse"));
		if (pinfo.powers.throwableBomb) {
			bomb->addComponent<lif::AxisMoving>(*bomb, lif::conf::player::DEFAULT_SPEED * 1.5,
				player->get<lif::AxisMoving>()->getPrevDirection());
			bomb->setPosition(static_cast<sf::Vector2f>(lif::tile2(player->getPosition()))
					* static_cast<float>(lif::TILE_SIZE));
		}
		tbspawned.emplace_back(bomb);
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
				tbspawned.emplace_back(spawned);
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

		// If it's a Grabbable, only give points if actually grabbed and not just expired
		const auto grabbable = e->get<lif::Grabbable>();
		if (grabbable != nullptr && !grabbable->isGrabbed())
			return;

		// Give and spawn points
		auto target = scored->getTarget();
		if (target < 0) {
			lm.addScoreToAll(scored->getPointsGiven());
			scored->givePoints();
		} else {
			lm.addScore(target, scored->givePoints());
		}

		// Apply absorb
		if (e->get<lif::Absorbable>() != nullptr) {
			for (int i = 0; i < lif::MAX_PLAYERS; ++i) {
				if (target >= 0 && target != i + 1) continue;
				auto& player = lm.getPlayer(i + 1);
				if (player == nullptr) continue;
				auto& powers = player->getPowers();
				if (powers.absorb > 0 && ++powers.absorbKillCount >= 3 - powers.absorb) {
					powers.absorbKillCount = 0;
					player->get<lif::Lifed>()->decLife(-1);
					tbspawned.emplace_back(new lif::AbsorbFX(e->getPosition(), player));
				}
			}
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
		tbspawned.emplace_back(points);
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
	lif::game_logic::bombDeployLogic,
	lif::game_logic::bonusGrabLogic,
	lif::game_logic::scoredKillablesLogic,
	lif::game_logic::spawningLogic
};
