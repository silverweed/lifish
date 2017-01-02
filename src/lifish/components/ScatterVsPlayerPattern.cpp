#include "ScatterVsPlayerPattern.hpp"
#include "Clock.hpp"
#include "FreeSighted.hpp"
#include "FreeBullet.hpp"
#include "Player.hpp"
#include <cmath>

using Game::ScatterVsPlayerPattern;

ScatterVsPlayerPattern::ScatterVsPlayerPattern(Game::Entity& owner, const Game::BulletInfo& bullet)
	: Game::ShootingPattern(owner, bullet)
{
	shootClock = addComponent(new Game::Clock(*this));
}

Game::Entity* ScatterVsPlayerPattern::init() {
	Game::Component::init();

	sighted = owner.get<Game::FreeSighted>();
	if (sighted == nullptr)
		throw std::invalid_argument("Owner of ScatterVsPlayerPattern has no FreeSighted!");
	
	return this;
}

void ScatterVsPlayerPattern::update() {
	Game::Component::update();
	if (!positionLocked) {
		// Find out player position; will keep using that until reset.
		auto player = sighted->nearest<Game::Player>().lock();
		if (player == nullptr) {
			setActive(false);
			return;
		}
		playerAngle = _calcAngle(player->getPosition() 
				+ sf::Vector2f(Game::TILE_SIZE / 2, Game::TILE_SIZE / 2));
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

Game::Angle ScatterVsPlayerPattern::_calcAngle(const sf::Vector2f& pos) const {
	const auto mpos = owner.getPosition();
	return Game::radians(Game::PI / 2. + std::atan2(pos.y - mpos.y, pos.x - mpos.x));
}

void ScatterVsPlayerPattern::_reset() {
	shootClock->restart();
	shotsFired = 0;
	positionLocked = false;
}

void ScatterVsPlayerPattern::_shoot() {
	// Fire a bullet in a random direction inside the cone with angle
	// `scatterAngle` and centered towards `playerPos`.
	playerAngle = _calcAngle(sighted->nearest<Game::Player>().lock()->getPosition());
	std::uniform_real_distribution<double> scatter(-scatterAngle.asRadians() / 2, scatterAngle.asRadians() / 2);
	addSpawned(new Game::FreeBullet(owner.getPosition(), 
				playerAngle + Game::radians(scatter(Game::rng)), bullet, &owner));
}
