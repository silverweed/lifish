#pragma once

#include "Bullet.hpp"

namespace Game {

/**
 * The Boss's bullets: differently from regular bullets,
 * a BossBullet is transparent to walls (but not enemies)
 * and can move in any direction, not just along x or y.
 */
class BossBullet : public Game::Bullet {
	/** The angle this bullet was thrown at (0 to 2*M_PI);
	 *  angle == 0 is the UP direction, rotating CW
	 */
	double angle;
public:
	BossBullet(const sf::Vector2f& pos, double angle);
};

}
