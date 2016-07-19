#pragma once

#include <exception>
#include "Component.hpp"
#include "game.hpp"

namespace Game {

/**
 * Something which is worth points
 */
class Scored : public Game::Component {
protected:
	const int pointsGiven;
	bool givenPoints = false;
	/** The id of the player who points are given to */
	short target = -1;

public:
	explicit Scored(Game::Entity *const owner, const int points) 
		: Game::Component(owner)
		, pointsGiven(points)
	{}

	int getPointsGiven() const { return pointsGiven; }

	/** @return whether this entity already gave points to the player or not */
	bool hasGivenPoints() const { return givenPoints; }
	int givePoints() {
		givenPoints = true;
		return pointsGiven;
	}

	short getTarget() const { return target; }
	/** Sets this component to give points to `t`-th player */
	void setTarget(unsigned short t) {
		if (target != -1) return; // can only set target once
		if (0 >= t || t > Game::MAX_PLAYERS)
			throw std::logic_error("setTarget set to a number greater than MAX_PLAYERS or < 1!");
		target = t;
	}
};

}
