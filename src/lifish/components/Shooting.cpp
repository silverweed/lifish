#include "Shooting.hpp"
#include "Clock.hpp"
#include "Bullet.hpp"
#include "AxisMoving.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include <exception>

using Game::Shooting;

const sf::Time Shooting::SHOOT_FRAME_TIME = sf::milliseconds(250); 

Shooting::Shooting(Game::Entity& owner, const Attack& attack)
	: Game::Component(owner)
	, attackAlign(-1.f, -1.f)
	, attack(attack) 
{
	position = owner.getPosition();
	rechargeClock = addComponent(new Game::Clock(*this));
}

Game::Entity* Shooting::init() {
	// optional
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
			moving->setDashing(4);
		}
		return nullptr;
	}

	if (dir == Game::Direction::NONE) {
		if (ownerMoving == nullptr)
			throw std::logic_error("Called shoot(Direction::NONE) on a non-AxisMoving owner!");
		
		shooting = true;
		if (attack.type & Game::AttackType::BLOCKING) {
			if (ownerMoving != nullptr)
				ownerMoving->block(attack.blockTime);
		}
		rechargeClock->restart();
		return new Game::AxisBullet(getPosition(), &owner, ownerMoving->getDirection(), attack);
	}

	shooting = true;
	if (attack.type & Game::AttackType::BLOCKING) {
		if (ownerMoving != nullptr)
			ownerMoving->block(attack.blockTime);
	}
	rechargeClock->restart();
	return new Game::AxisBullet(getPosition(), &owner, dir, attack);
}

Game::FreeBullet* Shooting::shoot(double angle) {
	if (attack.type & Game::AttackType::CONTACT)
		throw std::logic_error("Called shoot(angle) for a CONTACT attack!");

	shooting = true;
	rechargeClock->restart();
	return new Game::FreeBullet(getPosition(), &owner, angle, attack);
}

bool Shooting::isRecharging() const {
	return attack.fireRate > 0 && 
		rechargeClock->getElapsedTime().asSeconds() < 1./(fireRateMult * attack.fireRate);
}

void Shooting::update() {
	Game::Component::update();
	if (shooting && rechargeClock->getElapsedTime() > SHOOT_FRAME_TIME)
		shooting = false;
}

void Shooting::setFireRateMult(float fr) {
	if (fr <= 0)
		throw std::invalid_argument("Fire rate multiplier cannot be <= 0!");
	fireRateMult = fr; 
}

void Shooting::setPosition(const sf::Vector2f& pos) {
	Game::Component::setPosition(pos);
	manualPosition = true;
}

sf::Vector2f Shooting::getPosition() const {
	return manualPosition ? position : owner.getPosition() + offset;
}
