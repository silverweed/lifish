#pragma once

#include "Component.hpp"

namespace Game {

class Shooting : public Game::Component {
public:
	/** The different types of attacks.
	 *  SIMPLE: the enemy shoots a single bullet at a time
	 *  CONTACT: the enemy doesn't shoot, but hurts on contact.
	 *  RANGED: the enemy's attack has a limited range. If paired
	 *          with CONTACT, means the attack is "dashing", and
	 *          the dash triggers withing range (use -1 for "infinite").
	 *          __Note__: the "dashing" attack logic is actually performed 
	 *          by ai_follow_dash, independently from attack.type;
	 *          however, one must also specify this in attack.type so
	 *          that the attack sound is played correctly (i.e. when
	 *          the enemy starts dashing rather than when he contacts
	 *          the player).
	 *  BLOCKING: standalone, means that the enemy shoots continuously
	 *            and stops moving while shooting; paired with SIMPLE,
	 *            means the enemy shoots a single bullet and stops for
	 *            `attack.blockTime` after shooting.
	 */
	enum AttackType {
		SIMPLE   = 1,
		CONTACT  = 1 << 1,
		RANGED   = 1 << 2,
		BLOCKING = 1 << 3
	};

protected:
	/** Used by CONTACT attack AI */
	sf::Vector2i attackAlign;

public:
	AttackType type;

	/** Cooldown is 1/fireRate s. If AI is ai_follow_dash, determines
	 *  the cooldown between two dashes.
	 */
	float fireRate;
	
	/** If attacktype is SIMPLE | BLOCKING, this is the time
	 *  the enemy stops after shooting (should be more than shootFrameTime);
	 */
	sf::Time blockTime;
	
	/** id of the shot bullets */
	unsigned short id;

	unsigned short damage;
	
	/** Projectile speed, in units of Bullet::BASE_SPEED */
	float speed;

	/** If attack is RANGED, bullet dies after `range` tiles from source */
	short range;

	explicit Shooting(Game::Entity *const owner)
		: Game::Component(owner)
		, attackAlign(-1.f, -1.f) {}
};

}
