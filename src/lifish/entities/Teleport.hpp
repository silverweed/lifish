#pragma once

#include "Entity.hpp"

namespace lif {

class Animated;
class Collider;
class TeleportSystem;

/**
 * A teleport can be used both by players and enemies.
 */
class Teleport : public lif::Entity {

	friend class lif::TeleportSystem;

	static constexpr unsigned int N_ANIM_FRAMES = 8;

	bool disabled = false;
	/** Used by Spawning to spawn flashes */
	bool mustSpawnFlash = false;

	lif::Collider *collider = nullptr;
	lif::Animated *animated = nullptr;
public:
	explicit Teleport(const sf::Vector2f& pos);

	void triggerWarpFx();
};

}
