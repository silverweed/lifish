#include "Shooting.hpp"
#include <exception>

using Game::Shooting;

Shooting::Shooting(Game::Entity *const owner, const Attack& attack)
	: Game::Component(owner)
	, attackAlign(-1.f, -1.f)
	, attack(attack) 
{
	rechargeClock = addComponent(new Game::Clock(this));
	ownerMoving = owner->get<Game::AxisMoving>();
}

Game::AxisBullet* Shooting::shoot(Game::Direction dir) {
	if (attack.type & Game::AttackType::CONTACT) {
		rechargeClock->restart();
		if (attack.type & Game::AttackType::RANGED) {
			auto moving = owner->get<Game::Moving>();
			if (moving == nullptr)
				throw std::logic_error("Called shoot() for a dashing attack on a non-Moving owner!");
			moving->setDashing(true);
		}
		return nullptr;
	}

	if (dir == Game::Direction::NONE) {
		if (ownerMoving == nullptr)
			throw std::logic_error("Called shoot(Direction::NONE) on a non-AxisMoving owner!");
		
		shooting = true;
		rechargeClock->restart();
		return new Game::AxisBullet(owner, ownerMoving->getDirection(), attack);
	}

	shooting = true;
	rechargeClock->restart();
	return new Game::AxisBullet(owner, dir, attack);
}

Game::FreeBullet* Shooting::shoot(double angle) {
	if (attack.type & Game::AttackType::CONTACT)
		throw std::logic_error("Called shoot(angle) for a CONTACT attack!");

	shooting = true;
	rechargeClock->restart();
	return new Game::FreeBullet(owner, angle, attack);
}

bool Shooting::isRecharging() const {
	return attack.fireRate > 0 && rechargeClock->getElapsedTime().asSeconds() < 1./attack.fireRate;
}
