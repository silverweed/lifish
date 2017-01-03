#include "Shooting.hpp"
#include "Clock.hpp"
#include "Bullet.hpp"
#include "AxisMoving.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include "GameCache.hpp"
#include "Sounded.hpp"
#include "utils.hpp"
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
		shooting = true;
		Game::cache.playSound(owner.get<Game::Sounded>()->getSoundFile("attack"));
		rechargeClock->restart();
		attackAlign = Game::tile(owner.getPosition());
		if (ownerMoving != nullptr) {
			switch (ownerMoving->getDirection()) {
			case Game::Direction::UP: --attackAlign.y; break;
			case Game::Direction::DOWN: ++attackAlign.y; break;
			case Game::Direction::LEFT: --attackAlign.x; break;
			case Game::Direction::RIGHT: ++attackAlign.x; break;
			default: break;
			}
		}
		if (attack.type & Game::AttackType::RANGED) {
			auto moving = ownerMoving == nullptr ? owner.get<Game::Moving>() : ownerMoving;
			if (moving == nullptr)
				throw std::logic_error("Called shoot() for a dashing attack on a non-Moving owner!");
			moving->setDashing(4);
		}
		return nullptr;
	}

	if (dir == Game::Direction::NONE) {
		if (ownerMoving == nullptr)
			throw std::logic_error("Called shoot(Direction::NONE) on a non-AxisMoving owner!");
		
		if (attack.type & Game::AttackType::BLOCKING) {
			if (ownerMoving != nullptr)
				ownerMoving->block(attack.blockTime);
		}
		shooting = true;
		rechargeClock->restart();
		auto bullet = new Game::AxisBullet(getPosition(), ownerMoving->getDirection(), attack.bullet, &owner);
		Game::cache.playSound(bullet->get<Game::Sounded>()->getSoundFile("shot"));
		return bullet;
	}

	if (attack.type & Game::AttackType::BLOCKING) {
		if (ownerMoving != nullptr)
			ownerMoving->block(attack.blockTime);
	}
	shooting = true;
	rechargeClock->restart();
	auto bullet = new Game::AxisBullet(getPosition(), dir, attack.bullet, &owner);
	Game::cache.playSound(bullet->get<Game::Sounded>()->getSoundFile("shot"));
	return bullet;
}

Game::FreeBullet* Shooting::shoot(Game::Angle angle) {
	if (attack.type & Game::AttackType::CONTACT)
		throw std::logic_error("Called shoot(angle) for a CONTACT attack!");

	shooting = true;
	rechargeClock->restart();
	auto bullet = new Game::FreeBullet(getPosition(), angle, attack.bullet, &owner);
	Game::cache.playSound(bullet->get<Game::Sounded>()->getSoundFile("shot"));
	return bullet;
}

bool Shooting::isRecharging() const {
	return attack.fireRate > 0 && 
		rechargeClock->getElapsedTime().asSeconds() < 1. / (fireRateMult * attack.fireRate);
}

void Shooting::update() {
	Game::Component::update();
	if (shooting && rechargeClock->getElapsedTime() > SHOOT_FRAME_TIME) {
		shooting = false;
	}
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
