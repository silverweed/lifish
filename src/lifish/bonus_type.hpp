#pragma once

#include <string>

namespace Game {

class LevelManager;
class Player;

// This enum is not `enum class` because it's used by classes like SidePanel in a 'comparable' way
// or as an array index.
enum BonusType : unsigned {
	// "Permanent" bonuses
	MAX_BOMBS,
	QUICK_FUSE,
	MAX_RANGE,
	SHIELD,
	SPEEDY,
	// One-time bonuses
	ZAPPER,
	SUDDEN_DEATH,
	HEALTH_SMALL,
	HEALTH_FULL  
};

std::string bonusToString(BonusType type);
void triggerBonus(Game::LevelManager& lm, BonusType type, Game::Player& player);

}
