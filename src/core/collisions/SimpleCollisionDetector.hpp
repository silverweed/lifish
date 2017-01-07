#pragma once

#include "CollisionDetector.hpp"

namespace Game {

/**
 * This class implements a quadratic collision check on colliding entities of `group`.
 */
class SimpleCollisionDetector : public Game::CollisionDetector {
public:
	explicit SimpleCollisionDetector(Game::EntityGroup& group, const sf::FloatRect& levelLimit);

	void update() override;
};

}
