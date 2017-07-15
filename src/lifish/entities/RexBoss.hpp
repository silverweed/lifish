#pragma once

#include "Boss.hpp"

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
	} state = State::START;

	enum class AtkType {
		STOMP,
		FLAME,
		MISSILES,
		N_ATTACKS
	} atkType;

	bool wasBlocked = false;
	/** Incremented whenever we are aligned, used to decide when to attack */
	int steps = 0;

	lif::AxisMoving *moving = nullptr;
	lif::Clock *animClock = nullptr,
	           *attackClock = nullptr;
	lif::FreeSighted *sighted = nullptr;
	
	void _kill() override;
	void _updateStart();
	void _updateWalking();
	void _updateAttacking();
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
