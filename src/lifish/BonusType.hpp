#pragma once

namespace Game {

// This enum is not `enum class` because it's used by classes like SidePanel in a 'comparable' way
// or as an array index.
enum BonusType : unsigned {
	// "Permanent" bonuses
	MAX_BOMBS    = 0,
	QUICK_FUSE   = 1,
	MAX_RANGE    = 2,
	SHIELD       = 3,
	SPEEDY       = 4,
	// One-time bonuses
	ZAPPER       = 5,
	SUDDEN_DEATH = 6,
	HEALTH_SMALL = 7,
	HEALTH_FULL  = 8
};

}
