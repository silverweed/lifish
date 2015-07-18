#pragma once
/**
 * A bullet
 */

#include "MovingEntity.hpp"

namespace Game {

class Bullet : public Game::MovingEntity {
	/** If 1 => this bullet has the same animation for any direction, up to 8.
	 *  If 2 => this bullet has 2 different animations when traveling UP/DOWN or
	 *          LEFT/RIGHT, up to 4 per direction.
	 *  If 4 => this bullet has different animations for each direction, up to 2.
	 *  The effective number of frames is established by nMotionFrames.
	 */
	unsigned short directionality = 1;
	
	/** How many tiles does this bullet travel; -1 means infinite. */
	short range;
	unsigned short damage;

	unsigned short nMotionFrames = 1,  // up to 2
		       nDestroyFrames = 4; // up to 5

	bool _isTransparentTo(const Entity *const) const;
public:
	constexpr static float BASE_SPEED = 150.f;

	Bullet(const sf::Vector2f& pos, unsigned short id, float speed, unsigned short damage, short range = -1);
};

}
