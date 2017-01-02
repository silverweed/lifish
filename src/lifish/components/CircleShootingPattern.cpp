#include "CircleShootingPattern.hpp"
#include "FreeBullet.hpp"
#include "Clock.hpp"
#include "core.hpp"

using Game::CircleShootingPattern;

CircleShootingPattern::CircleShootingPattern(Game::Entity& owner, const Game::BulletInfo& bullet)
	: Game::ShootingPattern(owner, bullet)
{
	shootClock = addComponent(new Game::Clock(*this));
}

void CircleShootingPattern::update() {
	Game::Component::update();

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
		shootAngle = Game::degrees(dist(Game::rng));
	} else {
		shootAngle = Game::degrees(0);
	}
}

void CircleShootingPattern::_shoot() {
	const auto delta = Game::radians(2 * Game::PI / bulletsPerShot);
	// Shoot first bullet towards `shootAxis`'s direction
	auto angle = shootAngle;
	for (unsigned short i = 0; i < bulletsPerShot; ++i) {
		addSpawned(new Game::FreeBullet(owner.getPosition(), angle, bullet, &owner));
		angle += delta;
	}
	shootAngle += rotationPerShot;
}
