#pragma once

#include "Component.hpp"

namespace Game {

class Clock;
class Collider;

/** 
 * An object moving in a generic direction. Use its child
 * AxisMoving for restricting motion along axes or its child
 * FreeMoving for generic 2d motion.
 */
class Moving : public Game::Component {
protected:
	const static sf::Time MAX_FRAME_TIME;

	/** The speed that was initially given when constructing this component */
	const float originalSpeed;
	float dashAmount = 0;
	float speed;

	bool moving = false;
	float distTravelled = 0;

	sf::Time blockTime;
	bool blocked = false;
	
	sf::Vector2f prevAlign;
	Game::Clock *frameClock = nullptr,
		    *blockClock = nullptr;
	Game::Collider *collider = nullptr;


	bool _collidesWithSolid() const;
	/** Checks if entity is blocked and unblocks it if block expired.
	 *  @return true if entity is still blocked, false otherwise.
	 */
	bool _handleBlock();	
	/** @return the actual speed of this entity, which is the summation of
	 *  the base `speed` and `dashAmount * originalSpeed`.
	 *  Always use this, not directly `speed`.
	 */
	float _effectiveSpeed() const;
public:
	explicit Moving(Game::Entity& owner, float speed);

	float getSpeed() const { return speed; }
	float getOriginalSpeed() const { return originalSpeed; }
	/** Sets the base speed of this entity. This differs from `setDashing`, as it is just a
	 *  temporary effect (used for example in DASHING shooting attacks).
	 *  If `relativeToOriginal` is true, then `speed` is interpreted as a multiplier to the
	 *  original speed (i.e. passing `speed = 0.5` will halve the speed.)
	 */
	void setSpeed(float speed, bool relativeToOriginal = false); 

	float getDistTravelled() const { return distTravelled; }
	void setDistTravelled(float d) { distTravelled = d; }

	/** Make this entity dash, i.e. temporarily increase its speed by `mult * originalSpeed`. 
	 *  Passing `0` as argument cancels the dash.
	 */
	void setDashing(float mult);
	bool isDashing() const { return dashAmount != 0; }

	/** Prevent this entity's owner to move for `duration`. 
	 *  Give sf::Time::Zero as argument to unblock.
	 */
	void block(sf::Time duration);
	bool isBlocked() const { return blocked; }

	virtual void move();
	virtual void stop();

	virtual bool isMoving() const { return moving; }

	virtual Game::Entity* init() override;
	virtual void update() override = 0;
};

}
