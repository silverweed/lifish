#pragma once

#include "ShootingPattern.hpp"
#include "Angle.hpp"

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
	void _shoot();
	void _reset() override;
public:
	sf::Time timeBetweenShots = sf::seconds(0.5);
	/** Self-deactivate after shooting this number of attacks. Negative means 'infinite'. */
	short consecutiveShots;
	/** Number of bullets per shot */
	short bulletsPerShot;
	/** Amplitude of the scatter cone. */
	lif::Angle scatterAngle;

	/** `owner` MUST have a FreeSighted component */
	explicit ScatterVsPlayerPattern(lif::Entity& owner, const lif::BulletInfo& bullet);

	lif::Entity* init() override;
	void update() override;
};

}
