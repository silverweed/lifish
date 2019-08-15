#include "AlienBoss.hpp"
#include "Animated.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "FreeSighted.hpp"
#include "HurtDrawProxy.hpp"
#include "Killable.hpp"
#include "Lifed.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "Shooting.hpp"
#include "ShootingPoint.hpp"
#include "Sounded.hpp"
#include "Time.hpp"
#include "conf/boss.hpp"
#include "shoot_utils.hpp"
#include <iostream>

using namespace lif::conf::boss::alien_boss;
using lif::AlienBoss;

AlienBoss::AlienBoss(const sf::Vector2f& pos)
	: lif::Boss(pos)
{
	addComponent<lif::Sounded>(*this,
		lif::sid("death"), lif::getAsset("sounds", "alienboss_death.ogg"),
		lif::sid("hurt"), lif::getAsset("sounds", "alienboss_hurt.ogg")
	);
	const sf::Vector2f size(3 * lif::TILE_SIZE, 3 * lif::TILE_SIZE);
	drawable = addComponent<lif::Drawable>(*this, *addComponent<lif::HurtDrawProxy>(*this));
	addComponent<lif::Lifed>(*this, LIFE);
	addComponent<lif::Scored>(*this, VALUE);
	_addDefaultCollider(size);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "alien_boss.png"));
	animated->addAnimation("idle", { sf::IntRect(0, 0, size.x, size.y) }, true);

	lif::Attack attack;
	attack.type = lif::AttackType::SIMPLE;
	attack.bulletId = 101;
	attack.fireRate = 1. / SHOOT_SHORT_INTERVAL.asSeconds();

	// Shooting points
	eyes[0] = addComponent<lif::ShootingPoint>(*this, attack, SIGHT_RADIUS);
	eyes[0]->setOffset(sf::Vector2f(17, 18));
	eyes[1] = addComponent<lif::ShootingPoint>(*this, attack, SIGHT_RADIUS);
	eyes[1]->setOffset(sf::Vector2f(50, 18));
}

void AlienBoss::update() {
	lif::Boss::update();

	shootT += lif::time.getDelta();

	if ((canShoot && shootT > CAN_SHOOT_INTERVAL)
		|| (!canShoot && shootT > CANNOT_SHOOT_INTERVAL * shootIntervalMul))
	{
		canShoot = !canShoot;
		shootT = sf::Time::Zero;
	}

	if (killable->isKilled() || eyes[0]->get<lif::Shooting>()->isRecharging()
		|| eyes[1]->get<lif::Shooting>()->isRecharging())
	{
		return;
	}

	if (canShoot) {
		for (auto eye : eyes)
			lif::shootToNearestPlayer(*eye);
	}
}
