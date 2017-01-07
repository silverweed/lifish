#pragma once

#include "CollisionDetector.hpp"

namespace lif {

/**
 * This class implements a quadratic collision check on colliding entities of `group`.
 */
class SimpleCollisionDetector : public lif::CollisionDetector {
public:
	explicit SimpleCollisionDetector(lif::EntityGroup& group, const sf::FloatRect& levelLimit);

	void update() override;
};

}
