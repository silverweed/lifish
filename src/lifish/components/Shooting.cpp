#include "Shooting.hpp"
#include "Clock.hpp"
#include "AxisMoving.hpp"
#include "GameCache.hpp"
#include "Sounded.hpp"
#include "BulletFactory.hpp"
#include "utils.hpp"
#include <exception>

using lif::Shooting;

const sf::Time SHOOT_FRAME_TIME = sf::milliseconds(250);

Shooting::Shooting(lif::Entity& owner, const lif::Attack& attack)
	: lif::BufferedSpawner(owner)
	, attackAlign(-1.f, -1.f)
	, attack(attack)
{
	_declComponent<Shooting>();
	position = owner.getPosition();
	rechargeClock = addComponent<lif::Clock>(*this);
}

lif::Entity* Shooting::init() {
	// optional
	ownerMoving = owner.get<lif::AxisMoving>();
	if (ownerMoving != nullptr) {
		ownerMoving->setOnDashChange([this] (float old, float _new) {
			if (old != 0 && _new == 0)
				rechargeClock->restart();
		});
	}
	return this;
}

void Shooting::shoot() {
	_contactAttack();
}

void Shooting::shoot(const sf::Vector2f& targetPos) {
	if (attack.type & lif::AttackType::CONTACT) {
		_contactAttack();
		return;
	}
	_checkBlock();
	addSpawned(_doShoot(lif::BulletFactory::create(attack.bulletId, getPosition(), targetPos, &owner)));
}

void Shooting::shoot(lif::Direction dir) {
	if (attack.type & lif::AttackType::CONTACT) {
		_contactAttack();
		return;
	}
	_checkBlock();
	addSpawned(_doShoot(lif::BulletFactory::create(attack.bulletId, getPosition(), dir, &owner)));
}

void Shooting::shoot(lif::Angle angle) {
	if (attack.type & lif::AttackType::CONTACT) {
		_contactAttack();
		return;
	}
	_checkBlock();
	addSpawned(_doShoot(lif::BulletFactory::create(attack.bulletId, getPosition(), angle, &owner)));
}

bool Shooting::isRecharging() const {
	return attack.fireRate > 0 &&
		rechargeClock->getElapsedTime().asSeconds() < 1. / (fireRateMult * attack.fireRate);
}

void Shooting::update() {
	lif::Component::update();
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
	lif::Component::setPosition(pos);
	manualPosition = true;
}

sf::Vector2f Shooting::getPosition() const {
	return manualPosition ? position : owner.getPosition() + offset;
}

std::unique_ptr<lif::Bullet> Shooting::_doShoot(std::unique_ptr<lif::Bullet>&& bullet) {
	shooting = true;
	lif::cache.playSound(bullet->get<lif::Sounded>()->getSoundFile("shot"));
	rechargeClock->restart();
	return std::move(bullet);
}

void Shooting::_contactAttack() {
	shooting = true;
	lif::cache.playSound(owner.get<lif::Sounded>()->getSoundFile("attack"));
	rechargeClock->restart();
	attackAlign = lif::tile(owner.getPosition());
	if (ownerMoving != nullptr) {
		switch (ownerMoving->getDirection()) {
		case lif::Direction::UP: --attackAlign.y; break;
		case lif::Direction::DOWN: ++attackAlign.y; break;
		case lif::Direction::LEFT: --attackAlign.x; break;
		case lif::Direction::RIGHT: ++attackAlign.x; break;
		default: break;
		}
	}
	if (attack.type & lif::AttackType::RANGED) {
		auto moving = ownerMoving == nullptr ? owner.get<lif::Moving>() : ownerMoving;
		if (moving == nullptr)
			throw std::logic_error("Called shoot() for a dashing attack on a non-Moving owner!");
		moving->setDashing(4);
	}
}

void Shooting::_checkBlock() {
	if (attack.type & lif::AttackType::BLOCKING) {
		if (ownerMoving != nullptr)
			ownerMoving->block(attack.blockTime);
	}
}
