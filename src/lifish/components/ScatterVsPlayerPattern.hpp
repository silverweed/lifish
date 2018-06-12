#pragma once

#include "Angle.hpp"
#include "ShootingPattern.hpp"

namespace lif {

class Clock;
class FreeSighted;

/**
 * Shoots a scatter of bullets towards the player (i.e. in a cone directed to it)
 */
class ScatterVsPlayerPattern : public lif::ShootingPattern {
	unsigned short shotsFired = 0;
	/** Whether the player's position was already found or not */
	bool positionLocked = false;
	/** The angle between the vertical axis and the player's position, clockwise.
	 *  Note that this angle only gets updated _once_per_reset_.
	 */
	lif::Angle playerAngle;
	lif::Clock *shootClock = nullptr;
	lif::FreeSighted *sighted = nullptr;

	/** @return The CW angle between the v-axis and the given position */
	lif::Angle _calcAngle(const sf::Vector2f& pos) const;
	void _shoot(const sf::Vector2f& target);
	void _reset() override;
public:
	sf::Time timeBetweenShots = sf::seconds(0.5);
	/** Self-deactivate after shooting this number of attacks. Negative means 'infinite'. */
	short consecutiveShots = -1;
	/** Number of bullets per shot */
	short bulletsPerShot = 1;
	/** Amplitude of the scatter cone. */
	lif::Angle scatterAngle = lif::radians(1);

	/** `owner` MUST have a FreeSighted component */
	explicit ScatterVsPlayerPattern(lif::Entity& owner, unsigned bulletId);

	lif::Entity* init() override;
	void update() override;
};

}
