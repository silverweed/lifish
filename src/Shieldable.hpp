#pragma once

#include "Chronometer.hpp"

namespace Game {

class Shieldable {
protected:
	int shieldTime = -1; // milliseconds; negative means "no shield"
	sftools::Chronometer shieldClock;

public:
	virtual void giveShield(int shieldMs) {
		shieldTime = shieldMs;
		shieldClock.restart();
	}

	bool MovingEntity::hasShield() const {
		return shieldTime > 0 && shieldClock.getElapsedTime().asMilliseconds() <= shieldTime;
	}
};

}
