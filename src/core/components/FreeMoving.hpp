#pragma once

#include "Moving.hpp"

namespace lif {

/**
 * An object moving in any direction. Does not align to grid.
 */
class FreeMoving : public lif::Moving {
	/** Normalized velocity vector */
	sf::Vector2f velocity;

public:
	explicit FreeMoving(lif::Entity& owner, float speed,
			sf::Vector2f velocity = sf::Vector2f(0.f, 0.f));

	sf::Vector2f getVelocity() const { return velocity; }
	void setVelocity(const sf::Vector2f& vel);

	void update() override;
};

}
