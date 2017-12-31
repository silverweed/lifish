#pragma once

#include "Boss.hpp"
#include "LevelManager.hpp"
#include "state_function.hpp"

namespace lif {

class Animated;
class Clock;
class BufferedSpawner;
class HurtDrawProxy;
class LightSource;

class MainframeBoss : public lif::Boss, public sf::Drawable {

	lif::LevelManager& lm;

	enum class AttackType {
		ROTATING_SURGE,
		LASERS,
		LIGHTNING_STORM,
		SHIELD,
		SPAWN_ZAPS,
		N_ATTACKS
	};

	/** Used for RotatingSurge attack */
	lif::Angle nextAttackAngle = lif::Angle::Zero;

	/** Used for lightning storm, zaps and lasers */
	unsigned nShots = 0;

	/** How many lasers will actually shoot (based on remaining health) */
	unsigned lasersNShots = 0;
	sf::Time lasersShootDelay;

	sf::CircleShape shieldSprite;

	lif::Clock *clock = nullptr,
	           *sparkClock = nullptr;
	lif::Animated *animated = nullptr;
	lif::BufferedSpawner *spawner = nullptr;
	lif::HurtDrawProxy *hurtDrawProxy = nullptr;
	lif::LightSource *lightSource = nullptr;
	
	lif::ai::StateFunction stateFunction = std::bind(&MainframeBoss::_updateIdle, this);

	void _resetIdleAnim();
	void _checkShieldCollision();

	lif::ai::StateFunction _updateIdle();
	lif::ai::StateFunction _updateSurgeEntering();
	lif::ai::StateFunction _updateSurgeWindup();
	lif::ai::StateFunction _updateSurgeRecover();
	lif::ai::StateFunction _updateLightningEntering();
	lif::ai::StateFunction _updateLightningWindup();
	lif::ai::StateFunction _updateLightningShooting();
	lif::ai::StateFunction _updateLightningRecover();
	lif::ai::StateFunction _updateShieldEntering();
	lif::ai::StateFunction _updateShieldWindup();
	lif::ai::StateFunction _updateShieldDamage();
	lif::ai::StateFunction _updateShieldRecover();
	lif::ai::StateFunction _updateLasersEntering();
	lif::ai::StateFunction _updateLasersShooting();
	lif::ai::StateFunction _updateLasersRecover();
	lif::ai::StateFunction _updateSpawnZapsEntering();
	lif::ai::StateFunction _updateSpawnZapsShooting();
	lif::ai::StateFunction _updateSpawnZapsRecover();
public:
	explicit MainframeBoss(const sf::Vector2f& pos, lif::LevelManager& lm);

	void update() override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
