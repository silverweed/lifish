#pragma once

#include "Direction.hpp"
#include "Entity.hpp"

namespace lif {

class Collider;
class LevelManager;

class Egg : public lif::Entity {

	lif::Collider *collider = nullptr;
public:
	explicit Egg(const sf::Vector2f& pos, lif::Direction dir,
			const lif::LevelManager& lm, unsigned short spawnedEnemyId);

	void update() override;
};

}
