#pragma once

#include "Clock.hpp"
#include "Entity.hpp"

namespace Game {

class Shieldable : public Game::Component {
protected:
	int shieldTime = -1; // milliseconds; negative means "no shield"
	Game::Clock<1> *shieldClock;

public:
	explicit Shieldable(Game::Entity *const owner)
		: Game::Component(owner)
	{
		shieldClock = addComponent(new Game::Clock<1>(this));
	}


	void giveShield(int shieldMs) {
		shieldTime = shieldMs;
		shieldClock->restart();
	}

	bool hasShield() const {
		return shieldTime > 0 && shieldClock->getClock()->getElapsedTime().asMilliseconds() <= shieldTime;
	}
};

}
