#pragma once

#include "Component.hpp"
#include "Clock.hpp"
#include "Collider.hpp"

namespace Game {

/** 
 * An object moving in a generic direction. Use its child
 * AxisMoving for restricting motion along axes or its child
 * FreeMoving for generic 2d motion.
 */
class Moving : public Game::Component {
protected:
	const static sf::Time MAX_FRAME_TIME;

	bool moving = false;
	bool dashing = false;
	float speed;
	const float originalSpeed;
	float distTravelled = 0;
	sf::Vector2f prevAlign;
	Game::Clock *frameClock = nullptr;
	Game::Collider *collider = nullptr;


	bool _collidesWithSolid() const;

public:
	explicit Moving(Game::Entity *const owner, float speed);

	float getSpeed() const { return speed; }
	void setSpeed(const float _speed) { speed = _speed; }

	float getDistTravelled() const { return distTravelled; }
	void setDistTravelled(float d) { distTravelled = d; }

	/** Make this entity dash, i.e. temporarily increase its speed by multiplying it by `mult`. 
	 *  Passing `false` as argument resets the speed to `originalSpeed`.
	 */
	void setDashing(bool d, float mult = 4);
	bool isDashing() const { return dashing; }

	virtual void move();
	virtual void stop();

	virtual bool isMoving() const { return moving; }

	virtual void update() override = 0;
};

}
