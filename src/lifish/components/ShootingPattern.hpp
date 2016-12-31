#pragma once

#include "Attack.hpp"
#include "BufferedSpawner.hpp"

namespace Game {

/**
 * This is the base abstract class for a component that performs some kind of shooting pattern with a given Attack.
 * This component starts deactivated by default (since it shoots whenever it's active).
 */
class ShootingPattern : public Game::BufferedSpawner {
protected:
	Game::BulletInfo bullet;

	virtual void _reset() {}
public:
	COMP_NOT_UNIQUE

	explicit ShootingPattern(Game::Entity& owner, Game::BulletInfo bullet)
		: Game::BufferedSpawner(owner)
		, bullet(bullet)
	{
		active = false;
	}

	void resetAndPlay() {
		_reset();
		setActive(true);
	}

	virtual void update() override = 0;
};

}
