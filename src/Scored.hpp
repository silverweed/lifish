#pragma once

namespace Game {

/**
 * Something which is worth points
 */
class Scored {
protected:
	int pointsGiven;

public:
	Scored(const int points) : pointsGiven(points) {}

	int getPointsGiven() const { return pointsGiven; }
};

}
