#pragma once

#include <array>
#include "game.hpp"
#include "Boss.hpp"
#include "Direction.hpp"

namespace lif {

class AxisMoving;
class Clock;
class FreeSighted;
class Collider;
class BufferedSpawner;

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
	unsigned missilesShot = 0;
	/** Contains the latest updated positions of living players, or {-1, -1} */
	std::array<sf::Vector2f, lif::MAX_PLAYERS> latestPlayersPos;
	std::vector<sf::Vector2f> missilesTargets;

	lif::Direction flameDirection = lif::Direction::NONE;

	lif::Clock *animClock = nullptr,
	           *attackClock = nullptr;
	lif::AxisMoving *moving = nullptr;
	lif::FreeSighted *sighted = nullptr;
	lif::Collider *stompCollider = nullptr;
	lif::BufferedSpawner *spawner = nullptr;
	
	void _kill() override;
	void _updateStart();
	void _updateWalking();
	void _updateAttacking();
	void _updateDying();
	void _updateStomp();
	void _updateFlame();
	void _updateMissiles();
	/** Seeks for players and updates their known position */
	void _updatePlayersPos();
	bool _playersNearby() const;
	bool _playerAhead() const;
	bool _isAhead(const sf::Vector2f& pos) const;
	bool _isNearby(const sf::Vector2f& pos) const;
	int _checkAttackCondition() const;
	/** Decides the next missiles' targets */
	void _calcMissilesPos();
	void _shootMissile();
public:
	explicit RexBoss(const sf::Vector2f& pos);

	void update() override;
};

}
