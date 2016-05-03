#pragma once

#include "Component.hpp"
#include <string>

namespace Game {

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
enum AttackType : unsigned int {
	SIMPLE   = 1,
	CONTACT  = 1 << 1,
	RANGED   = 1 << 2,
	BLOCKING = 1 << 3
};

bool stringToAttackType(const std::string& str, AttackType& type) {
	if (str == "simple") type = AttackType::SIMPLE;
	else if (str == "contact") type = AttackType::CONTACT;
	else if (str == "ranged") type = AttackType::RANGED;
	else if (str == "blocking") type = AttackType::BLOCKING;
	else return false;

	return true;
}

/** Information about how an entity attacks */
struct Attack {
	AttackType type;

	/** Bullet id */
	unsigned short id;

	/** Damage dealt */
	short damage;
	
	/** Projectile speed, in units of Bullet::BASE_SPEED */
	float speed;
	
	/** Cooldown is 1/fireRate s. If AI is ai_follow_dash, determines
	 *  the cooldown between two dashes.
	 */
	float fireRate;
	
	/** If attack is RANGED, bullet dies after `range` tiles from source */
	short range;
	
	/** If attacktype is SIMPLE | BLOCKING, this is the time
	 *  the enemy stops after shooting (should be more than shootFrameTime);
	 */
	sf::Time blockTime;
};

class Shooting : public Game::Component {
protected:
	/** Used by CONTACT attack AI */
	sf::Vector2i attackAlign;
	
	Attack attack;

public:
	/** id of the shot bullets */
	unsigned short id;

	const Attack& getAttack() const { return attack; }

	explicit Shooting(Game::Entity *const owner, const Attack& attack)
		: Game::Component(owner)
		, attackAlign(-1.f, -1.f)
		, attack(attack) {}
};

}
