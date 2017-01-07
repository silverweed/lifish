#include "ScatterVsPlayerPattern.hpp"
#include "Clock.hpp"
#include "FreeSighted.hpp"
#include "FreeBullet.hpp"
#include "Player.hpp"
#include <cmath>

using lif::ScatterVsPlayerPattern;

ScatterVsPlayerPattern::ScatterVsPlayerPattern(lif::Entity& owner, const lif::BulletInfo& bullet)
	: lif::ShootingPattern(owner, bullet)
{
	shootClock = addComponent(new lif::Clock(*this));
}

lif::Entity* ScatterVsPlayerPattern::init() {
	lif::Component::init();

	sighted = owner.get<lif::FreeSighted>();
	if (sighted == nullptr)
		throw std::invalid_argument("Owner of ScatterVsPlayerPattern has no FreeSighted!");
	
	return this;
}

void ScatterVsPlayerPattern::update() {
	lif::Component::update();
	if (!positionLocked) {
		// Find out player position; will keep using that until reset.
		auto player = sighted->nearest<lif::Player>().lock();
		if (player == nullptr) {
			setActive(false);
			return;
		}
		playerAngle = _calcAngle(player->getPosition()
				+ sf::Vector2f(lif::TILE_SIZE / 2, lif::TILE_SIZE / 2));
		positionLocked = true;
	}
	if (shootClock->getElapsedTime() > timeBetweenShots) {
		_shoot();
		if (++shotsFired == consecutiveShots)
			setActive(false);
		else
			shootClock->restart();
	}
}

lif::Angle ScatterVsPlayerPattern::_calcAngle(const sf::Vector2f& pos) const {
	const auto mpos = owner.getPosition();
	return lif::radians(lif::PI / 2. + std::atan2(pos.y - mpos.y, pos.x - mpos.x));
}

void ScatterVsPlayerPattern::_reset() {
	shootClock->restart();
	shotsFired = 0;
	positionLocked = false;
}

void ScatterVsPlayerPattern::_shoot() {
	// Fire a bullet in a random direction inside the cone with angle
	// `scatterAngle` and centered towards `playerPos`.
	playerAngle = _calcAngle(sighted->nearest<lif::Player>().lock()->getPosition());
	std::uniform_real_distribution<double> scatter(-scatterAngle.asRadians() / 2, scatterAngle.asRadians() / 2);
	addSpawned(new lif::FreeBullet(owner.getPosition(), 
				playerAngle + lif::radians(scatter(lif::rng)), bullet, &owner));
}
