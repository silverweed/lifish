#pragma once

#include "Entity.hpp"
#include "conf/bomb.hpp"

namespace lif {

class Animated;
class Spawning;

class DroppingBomb : public lif::Entity {

	lif::Animated *animated = nullptr;
	lif::Spawning *spawning = nullptr;

public:
	explicit DroppingBomb(const sf::Vector2f& pos,
			const lif::Entity *const source = nullptr,
			const sf::Time& fuseTime = lif::conf::bomb::DEFAULT_FUSE,
			const unsigned short radius = lif::conf::bomb::DEFAULT_RADIUS,
			bool isIncendiary = false);
};

}
