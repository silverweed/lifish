#include "ScatterVsPlayerPattern.hpp"
#include "BulletFactory.hpp"
#include "FreeBullet.hpp"
#include "FreeSighted.hpp"
#include "Player.hpp"
#include "Time.hpp"
#include <cmath>

using lif::ScatterVsPlayerPattern;

ScatterVsPlayerPattern::ScatterVsPlayerPattern(lif::Entity& owner, unsigned bulletId)
	: lif::ShootingPattern(owner, bulletId)
{
	_declComponent<ScatterVsPlayerPattern>();
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

	shootT += lif::time.getDelta();

	// Note: this pointer is only valid until next update
	auto player = sighted->nearest<lif::Player>();
	if (player == nullptr) {
		setActive(false);
		return;
	}

	if (shootT > timeBetweenShots) {
		_shoot(player->getPosition());
		if (++shotsFired == consecutiveShots)
			setActive(false);
		else
			shootT = sf::Time::Zero;
	}
}

lif::Angle ScatterVsPlayerPattern::_calcAngle(const sf::Vector2f& pos) const {
	const auto mpos = owner.getPosition();
	return lif::radians(lif::PI / 2. + std::atan2(pos.y - mpos.y, pos.x - mpos.x));
}

void ScatterVsPlayerPattern::_reset() {
	shootT = sf::Time::Zero;
	shotsFired = 0;
	positionLocked = false;
}

void ScatterVsPlayerPattern::_shoot(const sf::Vector2f& target) {
	// Fire a bullet in a random direction inside the cone with angle
	// `scatterAngle` and centered towards `playerPos`.
	const auto playerAngle = _calcAngle(target);
	const auto halfScatter = scatterAngle.asRadians() / 2;
	std::uniform_real_distribution<double> scatter(-halfScatter, halfScatter);

	addSpawned(lif::BulletFactory::create(bulletId, owner.getPosition(),
			playerAngle + lif::radians(scatter(lif::rng)), &owner));
}
