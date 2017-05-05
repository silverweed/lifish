#pragma once

#include <string>

namespace lif {

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
	THROWABLE_BOMB,
	ABSORB,
	ARMOR,
	// One-time bonuses
	ZAPPER,
	SUDDEN_DEATH,
	HEALTH_SMALL,
	HEALTH_FULL
};

std::string bonusToString(lif::BonusType type);
void triggerBonus(lif::LevelManager& lm, lif::BonusType type, lif::Player& player);

}
