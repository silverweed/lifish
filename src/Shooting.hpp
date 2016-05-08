#pragma once

#include "Component.hpp"
#include "Clock.hpp"
#include "Bullet.hpp"
#include "AxisMoving.hpp"
#include "Attack.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include <string>

namespace Game {

class Shooting : public Game::Component {
protected:
	/** Used by CONTACT attack AI */
	sf::Vector2i attackAlign;
	
	Attack attack;
	bool shooting = false;

	Game::Clock<1> *rechargeClock = nullptr;
	Game::AxisMoving *ownerMoving = nullptr;
	
public:
	explicit Shooting(Game::Entity *const owner, const Attack& attack);

	const Attack& getAttack() const { return attack; }

	/** Creates an AxisBullet described by `attack` and returns it. 
	 *  The callee must take care of its destruction.
	 *  If dir is NONE, the bullet is shot in the direction of its owner. 
	 *  In this case, the owner must have an AxisMoving component, or an exception is thrown.
	 */
	Game::AxisBullet* shoot(Game::Direction dir = Game::Direction::NONE);
	/** Creates a FreeBullet moving with angle `angle` (rad) from its owner.
	 *  Callee must take care of its destruction.
	 */
	Game::FreeBullet* shoot(double angle);
	bool isShooting() const { return shooting; }

	bool isRecharging() const;
};

}
