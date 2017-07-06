#pragma once

#include "Spawning.hpp"

namespace lif {

struct Attack;
class Shooting;

/** A ShootingPoint is a component with a sight and an AutoShooting.
 *  Can be used to give an entity multiple "weapons".
 */
class ShootingPoint : public lif::Spawning {
	sf::Vector2f offset;
	lif::Shooting *shooting = nullptr;

public:
	COMP_NOT_UNIQUE

	explicit ShootingPoint(lif::Entity& owner, const lif::Attack& attack, float visionRadius = -1);

	void setOffset(const sf::Vector2f& off) { offset = off; }

	/** @return The actual position of this component */
	sf::Vector2f getPosition() const override {
		return owner.getPosition() + offset;
	}

	bool shouldSpawn() const override;
	std::unique_ptr<lif::Entity> spawn() override;
};

}
