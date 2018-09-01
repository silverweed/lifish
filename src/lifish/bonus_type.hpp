#pragma once

#include <string>

namespace lif {

class LevelManager;
class Player;

// Order is important! When changed, also change assets/bonuses.png
enum class BonusType {
	// "Permanent" bonuses
	MAX_BOMBS,
	MAX_RANGE,
	QUICK_FUSE,
	SHIELD,
	SPEEDY,
	// One-time bonuses
	ZAPPER,
	SUDDEN_DEATH,
	HEALTH_SMALL,
	HEALTH_FULL,
	INCENDIARY_BOMB,
	THROWABLE_BOMB,
	ABSORB,
	ARMOR,
};

std::string bonusToString(lif::BonusType type);
void triggerBonus(lif::LevelManager& lm, lif::BonusType type, lif::Player& player);

}
