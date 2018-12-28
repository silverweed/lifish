#include "CircleShootingPattern.hpp"
#include "BulletFactory.hpp"
#include "FreeBullet.hpp"
#include "Time.hpp"
#include "core.hpp"

using lif::CircleShootingPattern;

CircleShootingPattern::CircleShootingPattern(lif::Entity& owner, unsigned bulletId)
	: lif::ShootingPattern(owner, bulletId)
{
	_declComponent<CircleShootingPattern>();
}

void CircleShootingPattern::update() {
	lif::Component::update();

	shootT += lif::time.getDelta();

	if (shootT > timeBetweenShots) {
		_shoot();
		if (++shotsFired == consecutiveShots)
			setActive(false);
		else
			shootT = sf::Time::Zero;
	}
}

void CircleShootingPattern::_reset() {
	shootT = sf::Time::Zero;
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
		addSpawned(lif::BulletFactory::create(bulletId, owner.getPosition(), angle, &owner));
		angle += delta;
	}
	shootAngle += rotationPerShot;
}
