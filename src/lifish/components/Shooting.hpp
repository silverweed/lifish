#pragma once

#include <exception>
#include <memory>
#include "Component.hpp"
#include "Attack.hpp"
#include "Direction.hpp"
#include "Angle.hpp"
#include "AxisBullet.hpp"

namespace lif {

class Clock;
class FreeBullet;
class AxisMoving;

/**
 * A Shooting is a component that manages a single Attack type.
 * It provides methods for creating an Bullet with that Attack and querying
 * for recharge time, along with tweaking the bullet's starting offset and
 * firerate multiplier.
 * A Shooting component is meant for the "regular entities' shooting stuff";
 * the shoot() methods are overloaded for both AxisBullets and FreeBullets,
 * and take care of the extra effects like blocking the attacker after the shot, etc.
 */
class Shooting : public lif::Component {
	
	bool manualPosition = false;

protected:
	bool shooting = false;
	float fireRateMult = 1;
	sf::Vector2f offset;

	lif::Clock *rechargeClock = nullptr;
	lif::AxisMoving *ownerMoving = nullptr;


	void _checkContact()
	std::unique_ptr<lif::Bullet> _doShoot(const sf::Vector2f& pos);
	void _contactAttack();
	
public:
	explicit Shooting(lif::Entity& owner, const Attack& attack);

	/** If attack is CONTACT and not RANGED, just reset the recharge clock and return nullptr.
	 *  If attack is also RANGED (i.e. "dashing"), also call setDashing(true) for the owner's
	 *  Moving component (throws if no Moving component is found.)
	 *  Else, create an AxisBullet described by `attack` and return it.
	 *  If dir is NONE, the bullet is shot in the direction of its owner.
	 *  In this case, the owner must have an AxisMoving component, or an exception is thrown.
	 *  NOTE: this method does NOT check whether this entity is recharging.
	 */
	std::unique_ptr<lif::Bullet> shoot(const sf::Vector2f& targetPos);
	std::unique_ptr<lif::Bullet> shoot(const sf::Vector2f& targetPos);
	std::unique_ptr<lif::Bullet> shoot(const sf::Vector2f& targetPos);

	bool isShooting() const { return shooting; }
	bool isRecharging() const;

	void setFireRateMult(float fr);
	float getFireRateMult() const { return fireRateMult; }

	void setPosition(const sf::Vector2f& pos) override;
	sf::Vector2f getPosition() const override;
	void setOffset(const sf::Vector2f& off) { offset = off; }

	lif::Entity* init() override;
	void update() override;
};

}
