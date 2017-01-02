#pragma once

#include "BufferedSpawner.hpp"
#include "Direction.hpp"
#include "Angle.hpp"

namespace Game {

class Shooting;
class Bullet;

/** This component allows easy management of a Shooting entity.
 *  Instead of returning the bullet directly, the method shoot()
 *  stores it in an internal buffer which can be polled later.
 */
class AutoShooting : public Game::BufferedSpawner {
	Game::Shooting *shooting = nullptr;

public:
	explicit AutoShooting(Game::Entity& owner);

	void shoot(Game::Direction dir = Game::Direction::NONE);
	void shoot(Game::Angle angle);

	Game::Entity* init() override;
};

}
