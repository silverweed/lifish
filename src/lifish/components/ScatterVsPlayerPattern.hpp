#pragma once

#include "ShootingPattern.hpp"
#include "Angle.hpp"

namespace Game {

class Clock;

/**
 * Shoots a scatter of bullets towards the player (i.e. in a cone directed to it)
 */
class ScatterVsPlayerPattern : public Game::ShootingPattern {
	unsigned short shotsFired = 0;
	Game::Clock *shootClock = nullptr;

	void _shoot();
	void _reset() override;
public:
	sf::Time timeBetweenShots = sf::seconds(1);
	/** Self-deactivate after shooting this number of attacks. Negative means 'infinite'. */
	short consecutiveShots;
	/** Number of bullets per shot */
	short bulletsPerShot;
	/** Amplitude of the scatter cone. */
	Game::Angle scatterAngle;

	/** `owner` MUST have a FreeSighted component */
	explicit ScatterVsPlayerPattern(Game::Entity& owner, const Game::BulletInfo& bullet);

	void update() override;
};

}
