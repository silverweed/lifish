#pragma once

#include <exception>
#include "Component.hpp"

namespace lif {

/**
 * Something which is worth points
 */
class Scored : public lif::Component {
protected:
	const int pointsGiven;
	bool givenPoints = false;
	/** The id of the player who points are given to */
	short target = -1;

public:
	explicit Scored(lif::Entity& owner, const int points);

	int getPointsGiven() const { return pointsGiven; }
	/** @return whether this entity already gave points to the player or not */
	bool hasGivenPoints() const { return givenPoints; }
	/** Tell this component that it already gave points to someone, so it doesn't do that twice. */
	int givePoints() {
		givenPoints = true;
		return pointsGiven;
	}

	short getTarget() const { return target; }
	/** Sets this component to give points to `t`-th player */
	void setTarget(short t);
};

}
