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
class CircleShootingPattern : Game::ShootingPattern {
	unsigned short shotsFired = 0;

	Game::Clock *shootClock = nullptr;

	void _shoot();
public:
	sf::Time timeBetweenShots;
	/** Self-deactivate after shooting this number of attacks. Negative means 'infinite'. */
	short consecutiveShots;
	/** Number of bullets per shot */
	short bulletsPerShot;
	/** Degrees to rotate shoot axis after a shot. */
	float rotationPerShot;
	/** Versor towards direction of first bullet shot */
	sf::Vector2f shootAxis;

	explicit CircleShootingPattern(Game::Entity& owner, Game::Attack attack,
			sf::Time timeBetweenShots, short consecutiveShots);

	/** Resets all counters and clocks */
	void reset();
	
	void update() override;
};

}
