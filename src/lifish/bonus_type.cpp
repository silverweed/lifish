#include "bonus_type.hpp"
#include "LevelManager.hpp"
#include "Player.hpp"
#include "Lifed.hpp"
#include "BreakableWall.hpp"
#include "Scored.hpp"
#include "Killable.hpp"
#include "Enemy.hpp"
#include "Bonusable.hpp"
#include "game.hpp"

using B = Game::BonusType;

std::string Game::bonusToString(BonusType type) {
	switch (type) {
	case MAX_BOMBS:
		return "Max Bombs Up";
	case QUICK_FUSE:
		return "Quick Fuse";
	case MAX_RANGE:
		return "Max Range Up";
	case SHIELD:
		return "Shield";
	case SPEEDY:
		return "Speedy";
	case ZAPPER:
		return "Zapper";
	case SUDDEN_DEATH:
		return "Sudden Death";
	case HEALTH_SMALL:
		return "Small Health";
	case HEALTH_FULL:
		return "Full Health";
	default:
		break;
	}

	return "Unknown Bonus";
}

void Game::triggerBonus(Game::LevelManager& lm, Game::BonusType type, Game::Player& player) {
	const auto powers = player.getInfo().powers;

	switch (type) {
	case B::MAX_BOMBS:
		if (powers.maxBombs < Game::Conf::Player::MAX_MAX_BOMBS)
			player.setMaxBombs(powers.maxBombs + 1);
		break;
	case B::QUICK_FUSE:
		if (powers.bombFuseTime == Game::Conf::Bomb::DEFAULT_FUSE)
			player.setBombFuseTime(Game::Conf::Bomb::QUICK_FUSE);
		break;
	case B::MAX_RANGE:
		if (powers.bombRadius < Game::Conf::Bomb::MAX_RADIUS)
			player.setBombRadius(powers.bombRadius + 1);
		break;
	case B::SHIELD:
		player.get<Game::Bonusable>()->giveBonus(B::SHIELD, Game::Conf::Bonus::SHIELD_DURATION);
		break;
	case B::SPEEDY:
		player.get<Game::Bonusable>()->giveBonus(B::SPEEDY, Game::Conf::Bonus::SPEEDY_DURATION);
		break;
	case B::ZAPPER:
		lm.getEntities().apply([] (Game::Entity *e) {
			auto brk = dynamic_cast<Game::BreakableWall*>(e);
			if (brk == nullptr) return;

			auto klb = brk->get<Game::Killable>();
			if (klb->isKilled()) return;

			const auto score = brk->get<Game::Scored>()->getPointsGiven();
			klb->kill();
			for (unsigned short i = 0; i < Game::score.size(); ++i)
				Game::score[i] += score;
		});
		break;
	case B::SUDDEN_DEATH:
		lm.getEntities().apply([] (Game::Entity *e) {
			auto enemy = dynamic_cast<Game::Enemy*>(e);
			if (enemy == nullptr) return;

			auto klb = enemy->get<Game::Killable>();
			if (klb->isKilled()) return;

			const auto score = enemy->get<Game::Scored>()->getPointsGiven();
			klb->kill();
			for (unsigned short i = 0; i < Game::score.size(); ++i)
				Game::score[i] += score;
		});
		break;
	case B::HEALTH_FULL:
		player.get<Game::Lifed>()->refillLife();
		break;
	case B::HEALTH_SMALL:
		player.get<Game::Lifed>()->decLife(-2);
		break;
	}
}
