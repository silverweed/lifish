#pragma once

#include "BufferedSpawner.hpp"
#include "Direction.hpp"
#include "Angle.hpp"

namespace lif {

class Shooting;
class Bullet;

/** This component allows easy management of a Shooting entity.
 *  Instead of returning the bullet directly, the method shoot()
 *  stores it in an internal buffer which can be polled later.
 */
class AutoShooting : public lif::BufferedSpawner {
	lif::Shooting *shooting = nullptr;

public:
	explicit AutoShooting(lif::Entity& owner);

	void shoot(lif::Direction dir = lif::Direction::NONE);
	void shoot(lif::Angle angle);

	lif::Entity* init() override;
};

}
