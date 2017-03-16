#include "GuidedMoving.hpp"
#include "Clock.hpp"
#include "utils.hpp"
#include <algorithm>

using lif::GuidedMoving;

GuidedMoving::GuidedMoving(lif::Entity& owner,
		const sf::Vector2f& start, const sf::Vector2f& end, sf::Time timeTaken,
		std::initializer_list<lif::GuidedMoving::ModFunc> modfuncs)
	: lif::Moving(owner, 0)
	, start(start)
	, end(end)
	, timeTaken(timeTaken)
	, modfuncs(modfuncs)
{
	_declComponent<GuidedMoving>();

	clock = addComponent<lif::Clock>(*this);
}

void GuidedMoving::update() {
	lif::Component::update();

	// Calculate this once here
	const auto tPerc = clock->getElapsedTime() / timeTaken;

	auto pos = _calcPathPos(tPerc);
	
	for (auto pair : modfuncs) {
		pos += _calcModFunc(pair.first, pair.second ? tPerc : std::min(1.0f, tPerc));
	}

	owner.setPosition(pos);
}

sf::Vector2f GuidedMoving::_calcPathPos(float perc) const {
	return perc > 1 ? end : start + (end - start) * perc;
}

sf::Vector2f GuidedMoving::_calcModFunc(const GuidedMoving::_ModFunc& f, float perc) const {
	const auto v = f(perc);

	// Angle between (end - start) and x axis
	const auto angle = lif::normalized(end - start).x;
	const float c = std::cos(angle),
	            s = std::sin(angle);
	
	// Tilt the resulting vector to use (end - start) as the horizontal axis
	return sf::Vector2f(v.x * c - v.y * s, v.x * s + v.y * c);
}

void GuidedMoving::reset() {
	clock->restart();
}

bool GuidedMoving::isAtEnd() const {
	return clock->getElapsedTime() > timeTaken;
}
