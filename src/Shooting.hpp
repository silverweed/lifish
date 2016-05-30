#pragma once

#include "Component.hpp"
#include "Clock.hpp"
#include "Bullet.hpp"
#include "AxisMoving.hpp"
#include "Attack.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"

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
	
	const sf::Vector2i& getAttackAlign() const { return attackAlign; }
	void setAttackAlign(const sf::Vector2i& aa) { attackAlign = aa; }

	/** If attack is CONTACT, just reset the recharge clock and return nullptr.
	 *  Else, create an AxisBullet described by `attack` and return it. 
	 *  The callee must take care of its destruction.
	 *  If dir is NONE, the bullet is shot in the direction of its owner. 
	 *  In this case, the owner must have an AxisMoving component, or an exception is thrown.
	 *  NOTE: this method does NOT check whether this entity is recharging.
	 */
	Game::AxisBullet* shoot(Game::Direction dir = Game::Direction::NONE);

	/** Creates a FreeBullet moving with angle `angle` (rad) from its owner.
	 *  Callee must take care of its destruction. 
	 *  Throws if attack is CONTACT.
	 *  NOTE: this method does NOT check whether this entity is recharging.
	 */
	Game::FreeBullet* shoot(double angle);
	bool isShooting() const { return shooting; }

	bool isRecharging() const;
};

}
