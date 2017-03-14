#include "CircleShootingPattern.hpp"
#include "FreeBullet.hpp"
#include "Clock.hpp"
#include "core.hpp"

using lif::CircleShootingPattern;

CircleShootingPattern::CircleShootingPattern(lif::Entity& owner, const lif::BulletInfo& bullet)
	: lif::ShootingPattern(owner, bullet)
{
	_declComponent<CircleShootingPattern>();
	shootClock = addComponent<lif::Clock>(*this);
}

void CircleShootingPattern::update() {
	lif::Component::update();

	if (shootClock->getElapsedTime() > timeBetweenShots) {
		_shoot();
		if (++shotsFired == consecutiveShots)
			setActive(false);
		else
			shootClock->restart();
	}
}

void CircleShootingPattern::_reset() {
	shootClock->restart();
	shotsFired = 0;
	if (randomizeShootAngle) {
		std::uniform_real_distribution<double> dist(0, 360);
		shootAngle = lif::degrees(dist(lif::rng));
	} else {
		shootAngle = lif::degrees(0);
	}
}

void CircleShootingPattern::_shoot() {
	const auto delta = lif::radians(2 * lif::PI / bulletsPerShot);
	// Shoot first bullet towards `shootAxis`'s direction
	auto angle = shootAngle;
	for (int i = 0; i < bulletsPerShot; ++i) {
		addSpawned(new lif::FreeBullet(owner.getPosition(), angle, bullet, &owner));
		angle += delta;
	}
	shootAngle += rotationPerShot;
}
