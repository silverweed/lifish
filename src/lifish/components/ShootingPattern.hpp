#pragma once

#include "Attack.hpp"
#include "BufferedSpawner.hpp"

namespace lif {

/**
 * This is the base abstract class for a component that performs some kind of shooting pattern with a given Attack.
 * This component starts deactivated by default (since it shoots whenever it's active).
 */
class ShootingPattern : public lif::BufferedSpawner {
protected:
	unsigned bulletId;

	virtual void _reset() {}
public:
	COMP_NOT_UNIQUE

	explicit ShootingPattern(lif::Entity& owner, unsigned bulletId)
		: lif::BufferedSpawner(owner)
		, bulletId(bulletId)
	{
		_declComponent<ShootingPattern>();
		setActive(false);
	}
	virtual ~ShootingPattern() {}

	void resetAndPlay() {
		_reset();
		setActive(true);
	}

	virtual void update() override = 0;
};

}
