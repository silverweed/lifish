#pragma once

#include "Bullet.hpp"
#include <string>

namespace lif {

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
	SIMPLE     = 1,
	CONTACT    = 1 << 1,
	RANGED     = 1 << 2,
	BLOCKING   = 1 << 3
};

inline bool stringToAttackType(const std::string& str, AttackType& type) {
	if (str == "simple")
		type = AttackType::SIMPLE;
	else if (str == "contact")
		type = AttackType::CONTACT;
	else if (str == "ranged")
		type = AttackType::RANGED;
	else if (str == "blocking")
		type = AttackType::BLOCKING;
	else
		return false;

	return true;
}

/** Information about how an entity attacks */
struct Attack {
	lif::AttackType type = lif::AttackType::SIMPLE;

	/** Damage dealt by the entity itself colliding with the player */
	int contactDamage = 1;

	/** The bullet's infoId */
	unsigned bulletId;

	/** Cooldown is 1/fireRate s. If AI is ai_follow_dash, determines
	 *  the cooldown between two dashes.
	 */
	float fireRate = 1;

	/** If attacktype is SIMPLE | BLOCKING, this is the time
	 *  the enemy stops after shooting (should be more than Shooting::SHOOT_FRAME_TIME);
	 */
	sf::Time blockTime;

	/** The max distance in pixels at which this entity attacks; -1 means infinite.
	 *  Note that this amount may differ from the bullet's range, but it should
	 *  always be <= than it, or the bullet may die before reaching the target.
	 */
	float range = -1;
};

}
