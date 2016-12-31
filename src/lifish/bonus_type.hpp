#pragma once

#include <string>

namespace Game {

class LevelManager;
class Player;

enum class BonusType {
	// "Permanent" bonuses
	MAX_BOMBS,
	QUICK_FUSE,
	MAX_RANGE,
	SHIELD,
	SPEEDY,
	INCENDIARY_BOMB,
	// One-time bonuses
	ZAPPER,
	SUDDEN_DEATH,
	HEALTH_SMALL,
	HEALTH_FULL  
};

std::string bonusToString(BonusType type);
void triggerBonus(Game::LevelManager& lm, BonusType type, Game::Player& player);

}
