#pragma once

#include <array>
#include "game.hpp"
#include "Boss.hpp"
#include "Direction.hpp"
#include "state_function.hpp"

namespace lif {

class Animated;
class AxisMoving;
class Clock;
class FreeSighted;
class Collider;
class BufferedSpawner;

class RexBoss : public lif::Boss {

	enum class AtkType {
		STOMP,
		FLAME,
		MISSILES,
		N_ATTACKS
	} atkType;

	bool wasBlocked = false;
	/** Incremented whenever we are aligned, used to decide when to attack */
	int steps = 0;
	unsigned missilesShot = 0;
	/** Contains the latest updated positions of living players, or {-1, -1} */
	std::array<sf::Vector2f, lif::MAX_PLAYERS> latestPlayersPos;
	std::vector<sf::Vector2f> missilesTargets;

	lif::Direction flameDirection = lif::Direction::NONE;

	lif::Clock *animClock = nullptr,
	           *attackClock = nullptr;
	lif::Animated *animated = nullptr;
	lif::AxisMoving *moving = nullptr;
	lif::FreeSighted *sighted = nullptr;
	lif::Collider *stompCollider = nullptr;
	lif::BufferedSpawner *spawner = nullptr;

	lif::ai::StateFunction stateFunction = std::bind(&RexBoss::_updateStart, this);
	const std::array<lif::ai::StateFunction, 3> atkStateFunctions = {{
		std::bind(&RexBoss::_updateStompEntering, this),
		std::bind(&RexBoss::_updateFlameEntering, this),
		std::bind(&RexBoss::_updateMissilesEntering, this),
	}};
	
	void _kill() override;

	lif::ai::StateFunction _updateStart();
	lif::ai::StateFunction _updateWalking();
	lif::ai::StateFunction _updateDying();
	lif::ai::StateFunction _updateStompEntering();
	lif::ai::StateFunction _updateStompWindup();
	lif::ai::StateFunction _updateStompDamage();
	lif::ai::StateFunction _updateStompRecover();
	lif::ai::StateFunction _updateFlameEntering();
	lif::ai::StateFunction _updateFlameWindup();
	lif::ai::StateFunction _updateFlameDamage();
	lif::ai::StateFunction _updateFlameRecover();
	lif::ai::StateFunction _updateMissilesEntering();
	lif::ai::StateFunction _updateMissilesWindup();
	lif::ai::StateFunction _updateMissilesDamage();
	lif::ai::StateFunction _updateMissilesRecover();
	lif::ai::StateFunction _updateAttackExiting();

	/** Seeks for players and updates their known position */
	void _updatePlayersPos();
	/** Decides the next missiles' targets */
	void _calcMissilesPos();
	void _shootMissile();

	bool _playersNearby() const;
	bool _playerAhead() const;
	bool _isAhead(const sf::Vector2f& pos) const;
	bool _isNearby(const sf::Vector2f& pos) const;
	int _checkAttackCondition() const;
public:
	explicit RexBoss(const sf::Vector2f& pos);

	void update() override;
};

}
