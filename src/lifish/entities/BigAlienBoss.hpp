#pragma once

#include "Boss.hpp"

namespace lif {

class Animated;
class AxisMoving;
class BufferedSpawner;
class EnergyBar;
class LevelManager;

class BigAlienBoss : public lif::Boss {

	const lif::LevelManager& lm;

	lif::BufferedSpawner *spawner = nullptr;
	lif::Animated *animated = nullptr;
	lif::AxisMoving *moving = nullptr;
	lif::EnergyBar *energyBar = nullptr;

	sf::Time atkT;

	sf::Vector2f _eggOffset() const;

	void _checkCollision(lif::Collider& coll);
	void _kill() override;
public:
	explicit BigAlienBoss(const sf::Vector2f& pos, const lif::LevelManager& lm);

	void update() override;
};

}
