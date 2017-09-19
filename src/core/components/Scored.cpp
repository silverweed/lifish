#include "Scored.hpp"
#include "game.hpp"

using lif::Scored;

Scored::Scored(lif::Entity& owner, const int points)
	: lif::Component(owner)
	, pointsGiven(points)
{
	_declComponent<Scored>();
}

void Scored::setTarget(short t) {
	if (target != -1) return; // can only set target once
	if (0 >= t || t > lif::MAX_PLAYERS)
		throw std::logic_error("setTarget set to a number greater than MAX_PLAYERS or < 1!");
	target = t;
}
