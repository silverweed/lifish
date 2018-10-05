#pragma once

#include "Boss.hpp"

namespace lif {

class Animated;
class AxisMoving;
class BufferedSpawner;
class Clock;
class LevelManager;

class BigAlienBoss : public lif::Boss {

	const lif::LevelManager& lm;

	lif::BufferedSpawner *spawner = nullptr;
	lif::Clock *attackClock = nullptr;
	lif::Animated *animated = nullptr;
	lif::AxisMoving *moving = nullptr;

public:
	explicit BigAlienBoss(const sf::Vector2f& pos, const lif::LevelManager& lm);

	void update() override;
};

}
