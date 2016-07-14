#pragma once

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
 *  AXIS_BOUND: the bullet travels along axes, therefore its instance
 *              will be an AxisBullet.
 */
enum AttackType : unsigned int {
	SIMPLE     = 1,
	CONTACT    = 1 << 1,
	RANGED     = 1 << 2,
	BLOCKING   = 1 << 3,
	AXIS_BOUND = 1 << 4
};

inline bool stringToAttackType(const std::string& str, AttackType& type) {
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
	
	/** If attack is RANGED, bullet dies after `range` tiles (or pixels) from source */
	union {
		short tileRange;
		float pixelRange;
	};
	bool rangeInTiles;
	
	/** If attacktype is SIMPLE | BLOCKING, this is the time
	 *  the enemy stops after shooting (should be more than shootFrameTime);
	 */
	sf::Time blockTime;
};

}
