#include "ScatterVsPlayerPattern.hpp"
#include "Clock.hpp"

using Game::ScatterVsPlayerPattern;

ScatterVsPlayerPattern::ScatterVsPlayerPattern(Game::Entity& owner, const Game::BulletInfo& bullet)
	: Game::ShootingPattern(owner, bullet)
{}

void ScatterVsPlayerPattern::update() {
	Game::Component::update();

	if (shootClock->getElapsedTime() > timeBetweenShots) {
		_shoot();
		if (++shotsFired == consecutiveShots)
			setActive(false);
		else
			shootClock->restart();
	}
}

void ScatterVsPlayerPattern::_reset() {

}

void ScatterVsPlayerPattern::_shoot() {

}
