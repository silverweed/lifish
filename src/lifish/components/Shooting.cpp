#include "Shooting.hpp"
#include <exception>

using Game::Shooting;

const sf::Time Shooting::SHOOT_FRAME_TIME = sf::milliseconds(250); 

Shooting::Shooting(Game::Entity& owner, const Attack& attack)
	: Game::Component(owner)
	, attackAlign(-1.f, -1.f)
	, attack(attack) 
{
	rechargeClock = addComponent(new Game::Clock(*this));
	blockClock = addComponent(new Game::Clock(*this));
}

Game::Entity* Shooting::init() {
	ownerMoving = owner.get<Game::AxisMoving>();
	return this;
}

Game::AxisBullet* Shooting::shoot(Game::Direction dir) {
	if (attack.type & Game::AttackType::CONTACT) {
		rechargeClock->restart();
		if (attack.type & Game::AttackType::RANGED) {
			auto moving = owner.get<Game::Moving>();
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
		if (attack.type & Game::AttackType::BLOCKING) {
			blocked = true;
			blockClock->restart();
		}
		rechargeClock->restart();
		return new Game::AxisBullet(&owner, ownerMoving->getDirection(), attack);
	}

	shooting = true;
	if (attack.type & Game::AttackType::BLOCKING) {
		blocked = true;
		blockClock->restart();
	}
	rechargeClock->restart();
	return new Game::AxisBullet(&owner, dir, attack);
}

Game::FreeBullet* Shooting::shoot(double angle) {
	if (attack.type & Game::AttackType::CONTACT)
		throw std::logic_error("Called shoot(angle) for a CONTACT attack!");

	shooting = true;
	rechargeClock->restart();
	return new Game::FreeBullet(&owner, angle, attack);
}

bool Shooting::isRecharging() const {
	return attack.fireRate > 0 && rechargeClock->getElapsedTime().asSeconds() < 1./attack.fireRate;
}

void Shooting::update() {
	Game::Component::update();
	if (blocked && blockClock->getElapsedTime() > attack.blockTime)
		blocked = false;
	if (shooting && rechargeClock->getElapsedTime() > SHOOT_FRAME_TIME)
		shooting = false;
}
