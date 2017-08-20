#pragma once

#include "Entity.hpp"

namespace lif {

class Animated;
class Collider;

/**
 * Spikes don't move. They activate when a player steps on them and
 * deal damage after a while. This damage CAN affect enemies (but they
 * do not trigger the spikes).
 */
class Spikes : public lif::Entity {

	bool damaging = false;

	lif::Animated *animated = nullptr;

	void _maybeTrigger(const lif::Collider& c);

public:
	explicit Spikes(const sf::Vector2f& pos);

	void update() override;
};

}
