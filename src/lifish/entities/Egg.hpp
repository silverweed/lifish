#pragma once

#include "Direction.hpp"
#include "Entity.hpp"

namespace lif {

class Animated;
class Collider;
class LevelManager;

class Egg : public lif::Entity {

	bool wasAnimationPlaying = true;

	lif::Collider *collider = nullptr;
	lif::Animated *animated = nullptr;

public:
	explicit Egg(const sf::Vector2f& pos, lif::Direction dir,
			const lif::LevelManager& lm, unsigned short spawnedEnemyId);

	void update() override;
};

}
