#include "bonus_type.hpp"
#include "BaseLevelManager.hpp"
#include "Player.hpp"
#include "Lifed.hpp"
#include "BreakableWall.hpp"
#include "Scored.hpp"
#include "Killable.hpp"
#include "Enemy.hpp"
#include "Bonusable.hpp"
#include "game.hpp"

using B = lif::BonusType;

std::string lif::bonusToString(lif::BonusType type) {
	switch (type) {
	case B::MAX_BOMBS:
		return "Max Bombs Up";
	case B::QUICK_FUSE:
		return "Quick Fuse";
	case B::MAX_RANGE:
		return "Max Range Up";
	case B::SHIELD:
		return "Shield";
	case B::SPEEDY:
		return "Speedy";
	case B::INCENDIARY_BOMB:
		return "Incendiary Bomb";
	case B::ZAPPER:
		return "Zapper";
	case B::SUDDEN_DEATH:
		return "Sudden Death";
	case B::HEALTH_SMALL:
		return "Small Health";
	case B::HEALTH_FULL:
		return "Full Health";
	default:
		break;
	}

	return "Unknown Bonus";
}

void lif::triggerBonus(lif::BaseLevelManager& lm, lif::BonusType type, lif::Player& player) {
	const auto powers = player.getInfo().powers;

	switch (type) {
	case B::MAX_BOMBS:
		if (powers.maxBombs < lif::conf::player::MAX_MAX_BOMBS)
			player.setMaxBombs(powers.maxBombs + 1);
		break;
	case B::QUICK_FUSE:
		if (powers.bombFuseTime == lif::conf::bomb::DEFAULT_FUSE)
			player.setBombFuseTime(lif::conf::bomb::QUICK_FUSE);
		break;
	case B::MAX_RANGE:
		if (powers.bombRadius < lif::conf::bomb::MAX_RADIUS)
			player.setBombRadius(powers.bombRadius + 1);
		break;
	case B::SHIELD:
		player.get<lif::Bonusable>()->giveBonus(B::SHIELD, lif::conf::bonus::SHIELD_DURATION);
		break;
	case B::SPEEDY:
		player.get<lif::Bonusable>()->giveBonus(B::SPEEDY, lif::conf::bonus::SPEEDY_DURATION);
		break;
	case B::INCENDIARY_BOMB:
		player.setIncendiaryBomb(true);
		break;
	case B::ZAPPER:
		lm.getEntities().apply([] (lif::Entity *e) {
			auto brk = dynamic_cast<lif::BreakableWall*>(e);
			if (brk == nullptr) return;

			auto klb = brk->get<lif::Killable>();
			if (klb->isKilled()) return;

			const auto score = brk->get<lif::Scored>()->getPointsGiven();
			klb->kill();
			for (unsigned i = 0; i < lif::score.size(); ++i)
				lif::score[i] += score;
		});
		break;
	case B::SUDDEN_DEATH:
		lm.getEntities().apply([] (lif::Entity *e) {
			auto enemy = dynamic_cast<lif::Enemy*>(e);
			if (enemy == nullptr) return;

			auto klb = enemy->get<lif::Killable>();
			if (klb->isKilled()) return;

			const auto score = enemy->get<lif::Scored>()->getPointsGiven();
			klb->kill();
			for (unsigned i = 0; i < lif::score.size(); ++i)
				lif::score[i] += score;
		});
		break;
	case B::HEALTH_FULL:
		player.get<lif::Lifed>()->refillLife();
		break;
	case B::HEALTH_SMALL:
		player.get<lif::Lifed>()->decLife(-2);
		break;
	}
}
