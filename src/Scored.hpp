#pragma once
/**
 * Something which is worth points
 */

namespace Game {

class Scored {
protected:
	int pointsGiven;

public:
	Scored(const int points) : pointsGiven(points) {}

	int getPointsGiven() const { return pointsGiven; }
};

}
