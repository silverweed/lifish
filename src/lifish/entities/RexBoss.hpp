#pragma once

#include "Boss.hpp"

namespace lif {

class AxisMoving;
class Clock;

class RexBoss : public lif::Boss {
	enum State {
		START,
		WALKING,
		SHOOTING,
	} state = State::START;

	bool wasBlocked = false;

	lif::AxisMoving *moving = nullptr;
	lif::Clock *animClock = nullptr,
	           *attackClock = nullptr;
	
	void _kill() override;
	void _updateStart();
	void _updateWalking();
	void _updateShooting();
public:
	explicit RexBoss(const sf::Vector2f& pos);

	void update() override;
};

}
