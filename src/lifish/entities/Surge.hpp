#pragma once

#include "Entity.hpp"
#include "Angle.hpp"
#include "LevelManager.hpp"

namespace lif {

class Animated;
class Clock;

class Surge : public lif::Entity {

	const lif::LevelManager& lm;

	lif::Angle rotPerSecond;

	lif::Animated *animated = nullptr;
	lif::Clock *clock = nullptr;
	
	/** Check if any player collides with the Surge. We must do this manually,
	 *  as the collision detection system doesn't support non-axis-aligned colliders.
	 */
	void _checkCollision();
public:
	explicit Surge(const sf::Vector2f& pos, const lif::Angle& rotPerSecond, const lif::LevelManager& lm);

	void update() override;
};

}
