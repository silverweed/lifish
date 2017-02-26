#pragma once

#include <string>

namespace lif {

class BaseLevelManager;
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

std::string bonusToString(lif::BonusType type);
void triggerBonus(lif::BaseLevelManager& lm, lif::BonusType type, lif::Player& player);

}
