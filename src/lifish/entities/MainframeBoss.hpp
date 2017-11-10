#pragma once

#include "Boss.hpp"
#include "LevelManager.hpp"
#include "state_function.hpp"

namespace lif {

class Clock;
class BufferedSpawner;

class MainframeBoss : public lif::Boss {

	const lif::LevelManager& lm;

	enum class AttackType {
		ROTATING_SURGE,
		LASERS,
		LIGHTNING_STORM,
		SHIELD,
		N_ATTACKS
	};

	lif::Clock *clock = nullptr;
	lif::BufferedSpawner *spawner = nullptr;
	
	lif::ai::StateFunction stateFunction = std::bind(&MainframeBoss::_updateIdle, this);

	lif::ai::StateFunction _updateIdle();
	lif::ai::StateFunction _updateRotatingSurge();
public:
	explicit MainframeBoss(const sf::Vector2f& pos, const lif::LevelManager& lm);

	void update() override;
};

}
