#pragma once

#include "Component.hpp"
#include "Clock.hpp"

namespace Game {

/** 
 * An object moving in a generic direction. Use its child
 * AxisMoving for restricting motion along axes.
 */
class Moving : public Game::Component {
protected:
	const static sf::Time MAX_FRAME_TIME;

	bool moving = false;
	float speed;
	float distTravelled = 0;
	Game::Clock<1> *frameClock = nullptr;

public:
	explicit Moving(Game::Entity *const owner, float speed);

	float getSpeed() const { return speed; }
	void setSpeed(const float _speed) { speed = _speed; }

	virtual void move();
	virtual void stop();

	virtual void update() override = 0;
};

}
