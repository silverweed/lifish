#pragma once

#include "Entity.hpp"

namespace lif {

class Clock;
class LightSource;

class Flare : public lif::Entity {

	const sf::Time burstTime;
	const sf::Time decayTime;

	bool bursting = true;
	lif::LightSource *light = nullptr;
	lif::Clock *lightClock = nullptr;

public:
	explicit Flare(sf::Time burstTime, sf::Time decayTime);

	void update() override;
};

}
