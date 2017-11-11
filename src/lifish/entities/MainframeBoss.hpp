#pragma once

#include "Boss.hpp"
#include "LevelManager.hpp"
#include "state_function.hpp"

namespace lif {

class Animated;
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

	/** Used for RotatingSurge attack */
	lif::Angle nextAttackAngle = lif::Angle::Zero;

	/** Used for lightning storm */
	unsigned nShots = 0;

	lif::Clock *clock = nullptr;
	lif::Animated *animated = nullptr;
	lif::BufferedSpawner *spawner = nullptr;
	
	lif::ai::StateFunction stateFunction = std::bind(&MainframeBoss::_updateIdle, this);

	void _resetIdleAnim();

	lif::ai::StateFunction _updateIdle();
	lif::ai::StateFunction _updateSurgeEntering();
	lif::ai::StateFunction _updateSurgeWindup();
	lif::ai::StateFunction _updateSurgeRecover();
	lif::ai::StateFunction _updateLightningEntering();
	lif::ai::StateFunction _updateLightningWindup();
	lif::ai::StateFunction _updateLightningShooting();
	lif::ai::StateFunction _updateLightningRecover();
public:
	explicit MainframeBoss(const sf::Vector2f& pos, const lif::LevelManager& lm);

	void update() override;
};

}
