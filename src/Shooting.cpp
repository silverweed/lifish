#include "Shooting.hpp"
#include <exception>

using Game::Shooting;

Shooting::Shooting(Game::Entity *const owner, const Attack& attack)
	: Game::Component(owner)
	, attackAlign(-1.f, -1.f)
	, attack(attack) 
{
	rechargeClock = addComponent(new Game::Clock<1>(this));
	ownerMoving = owner->get<Game::AxisMoving>();
}

Game::AxisBullet* Shooting::shoot(Game::Direction dir) {
	if (dir == Game::Direction::NONE) {
		if (ownerMoving == nullptr)
			throw std::invalid_argument("Called shoot(NONE) on a non-AxisMoving owner!");
		
		shooting = true;
		return new Game::AxisBullet(owner, ownerMoving->getDirection(), attack);
	}

	shooting = true;
	return new Game::AxisBullet(owner, dir, attack);
}

Game::FreeBullet* Shooting::shoot(double angle) {
	shooting = true;
	return new Game::FreeBullet(owner, angle, attack);
}

bool Shooting::isRecharging() const {
	return attack.fireRate > 0 && rechargeClock->getElapsedTime().asSeconds() < 1./attack.fireRate;
}
