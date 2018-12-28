#pragma once

#include "Angle.hpp"
#include "Attack.hpp"
#include "BufferedSpawner.hpp"
#include "Component.hpp"
#include "Direction.hpp"
#include <exception>
#include <memory>

namespace lif {

class FreeBullet;
class AxisMoving;

/**
 * A Shooting is a component that manages a single Attack type.
 * It provides methods for creating a Bullet with that Attack and querying
 * for recharge time, along with tweaking the bullet's starting offset and
 * firerate multiplier.
 * A Shooting component is meant for the "regular entities' shooting stuff";
 * the shoot() methods are overloaded for contact attacks, AxisBullets, GuidedBullets and FreeBullets,
 * and take care of the extra effects like blocking the attacker after the shot, etc.
 */
class Shooting : public lif::BufferedSpawner {

	bool manualPosition = false;

protected:
	/** Tile at which the owner shot the latest time. */
	sf::Vector2i attackAlign;

	lif::Attack attack;
	bool shooting = false;
	float fireRateMult = 1;
	sf::Vector2f offset;

	lif::AxisMoving *ownerMoving = nullptr;

	sf::Time rechargeT;


	void _checkBlock();
	std::unique_ptr<lif::Bullet> _doShoot(std::unique_ptr<lif::Bullet>&& bullet);
	void _contactAttack();

public:
	explicit Shooting(lif::Entity& owner, const Attack& attack);

	const lif::Attack& getAttack() const { return attack; }

	sf::Vector2i getAttackAlign() const { return attackAlign; }
	void setAttackAlign(const sf::Vector2i& aa) { attackAlign = aa; }

	/** Makes a contact attack. Recharge clock is reset and, if attack is also RANGED ("dashing" attack),
	 *  also call setDashing(true) for the owner's Moving component (throws if no Moving component is found).
	 */
	void shoot();
	/** Shoots towards `targetPos`. Bullet can be of any kind.
	 *  NOTE: this method does NOT check whether this entity is recharging.
	 */
	void shoot(const sf::Vector2f& targetPos);
	/** Shoots a bullet towards direction `dir`. The spawned bullet should be an AxisBullet
	*   or the behaviour is undefined.
	 *  If dir is NONE, the bullet is shot in the direction of its owner.
	 *  In this case, the owner must have an AxisMoving component, or an exception is thrown.
	 *  NOTE: this method does NOT check whether this entity is recharging.
	 */
	void shoot(lif::Direction dir);
	/** Shoots a bullet towards angle `angle` (starting from UP, clockwise).
	 *  The spawned bullet should b a FreeBullet, or the behaviour is undefined.
	 *  NOTE: this method does NOT check whether this entity is recharging.
	 */
	void shoot(lif::Angle angle);

	/** @return whether this component's owner has shot within the latest SHOOT_FRAME_TIME */
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
