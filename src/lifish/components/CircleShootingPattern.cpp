#include "CircleShootingPattern.hpp"
#include "Clock.hpp"
#include "core.hpp"

using Game::CircleShootingPattern;

CircleShootingPattern::CircleShootingPattern(Game::Entity& owner, Game::Attack attack)
	: Game::ShootingPattern(owner, attack)
	, shootAxis(0, 1)
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

void CircleShootingPattern::reset() {
	shootClock->restart();
	shotsFired = 0;
}

void CircleShootingPattern::_shoot() {
	const double angle = 2 * Game::PI / bulletsPerShot;
	// Shoot first bullet towards `shootAxis`'s direction
	for (unsigned short i = 0; i < bulletsPerShot; ++i) {
		shooter->shoot();
	}
}
