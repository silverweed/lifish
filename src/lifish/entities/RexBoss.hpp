#pragma once

#include "Boss.hpp"
#include "Direction.hpp"

namespace lif {

class AxisMoving;
class Clock;
class FreeSighted;
class Collider;

class RexBoss : public lif::Boss {
	enum class State {
		START,
		WALKING,
		ATTACKING,
		DYING
	} state = State::START;

	enum class AtkType {
		STOMP,
		FLAME,
		MISSILES,
		N_ATTACKS
	} atkType;

	enum class AtkState {
		ENTERING,
		WINDUP,
		DAMAGE,
		RECOVER,
		EXITING
	} atkState;

	bool wasBlocked = false;
	/** Incremented whenever we are aligned, used to decide when to attack */
	int steps = 0;

	lif::Direction flameDirection = lif::Direction::NONE;

	lif::AxisMoving *moving = nullptr;
	lif::Clock *animClock = nullptr,
	           *attackClock = nullptr;
	lif::FreeSighted *sighted = nullptr;
	lif::Collider *stompCollider = nullptr;
	
	void _kill() override;
	void _updateStart();
	void _updateWalking();
	void _updateAttacking();
	void _updateDying();
	void _updateStomp();
	void _updateFlame();
	void _updateMissiles();
	bool _playersNearby() const;
	bool _playerAhead() const;
	bool _isAhead(const lif::Collider& cld) const;
	bool _isNearby(const lif::Collider& cld) const;
	int _checkAttackCondition() const;
public:
	explicit RexBoss(const sf::Vector2f& pos);

	void update() override;
};

}
