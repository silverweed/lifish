#pragma once

#include <memory>
#include <queue>
#include "Spawning.hpp"
#include "Direction.hpp"

namespace Game {

class Shooting;
class Bullet;

/** This component allows easy management of a Shooting entity.
 *  Instead of returning the bullet directly, the method shoot()
 *  stores it in an internal buffer which can be polled later.
 */
class AutoShooting : public Game::Spawning {
	std::queue<std::unique_ptr<Game::Bullet>> latestShot;
	Game::Shooting *shooting = nullptr;

public:
	explicit AutoShooting(Game::Entity& owner);

	void shoot(Game::Direction dir = Game::Direction::NONE);
	void shoot(double angle);

	Game::Entity* init() override;

	bool shouldSpawn() const override;
	std::unique_ptr<Game::Entity> spawn() override;
};

}
