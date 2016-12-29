#pragma once

#include <SFML/System.hpp>
#include "ShootingPattern.hpp"

namespace Game {

class Clock;

/**
 * Shoots bullets in a circular fashion:
 *        |   o
 *   o    |
 * o + o  |o  +  o
 *   o    |
 *        |   o
 */
class CircleShootingPattern : public Game::ShootingPattern {
	unsigned short shotsFired = 0;

	Game::Clock *shootClock = nullptr;

	void _shoot();
public:
	sf::Time timeBetweenShots = sf::seconds(1);
	/** Self-deactivate after shooting this number of attacks. Negative means 'infinite'. */
	short consecutiveShots;
	/** Number of bullets per shot */
	short bulletsPerShot;
	/** Radians to rotate shoot axis after a shot. */
	float rotationPerShot;
	/** Angle of the first bullet (relative to vertical axis, clockwise) */
	float shootAngle;

	/** Note that, rather than passing all parameters in the constructor,
	 *  you just set it directly, as the useful variables are public for convenience.
	 */
	explicit CircleShootingPattern(Game::Entity& owner, const Game::BulletInfo& bullet);

	/** Resets all counters and clocks */
	void reset();
	
	void update() override;
};

}
