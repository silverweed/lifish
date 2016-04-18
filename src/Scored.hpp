#pragma once

#include "Component.hpp"

namespace Game {

/**
 * Something which is worth points
 */
class Scored : public Game::Component {
protected:
	const int pointsGiven;

public:
	explicit Scored(Game::Entity *const owner, const int points) 
		: pointsGiven(points), Game::Component(owner) {}

	int getPointsGiven() const { return pointsGiven; }
};

}
