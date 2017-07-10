#include "AlienBoss.hpp"
#include "Player.hpp"
#include "Killable.hpp"
#include "Collider.hpp"
#include "Sounded.hpp"
#include "Clock.hpp"
#include "ShootingPoint.hpp"
#include "FreeSighted.hpp"
#include "Drawable.hpp"
#include "Lifed.hpp"
#include "Scored.hpp"
#include "Animated.hpp"
#include "Shooting.hpp"
#include "conf/boss.hpp"
#include "shoot_utils.hpp"
#include <iostream>

using namespace lif::conf::boss::alien_boss;
using lif::AlienBoss;

AlienBoss::AlienBoss(const sf::Vector2f& pos)
	: lif::Boss(pos)
{
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("death", lif::getAsset("test", std::string("alienboss_death.ogg"))),
		std::make_pair("hurt", lif::getAsset("test", std::string("alienboss_hurt.ogg")))
	});
	const sf::Vector2f size(3 * lif::TILE_SIZE, 3 * lif::TILE_SIZE);
	_addDefaultCollider(size);
	addComponent<lif::Scored>(*this, VALUE);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("test", "alien_boss.png"));
	animated->addAnimation("idle", { sf::IntRect(0, 0, size.x, size.y) }, true);
	addComponent<lif::Lifed>(*this, LIFE);
	shootClock = addComponent<lif::Clock>(*this);

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
	lif::Entity::update();
	if (killable->isKilled() || eyes[0]->get<lif::Shooting>()->isRecharging()) return;
	
	if (shotsFired > 0 || shootClock->getElapsedTime() > SHOOT_INTERVAL) {
		for (auto eye : eyes)
			lif::shootToNearestPlayer(*eye);
		shotsFired = (shotsFired + 1) % N_SHOTS;
		if (shotsFired == 0)
			shootClock->restart();
	}
}
