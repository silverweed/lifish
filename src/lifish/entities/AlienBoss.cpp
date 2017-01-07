#include "AlienBoss.hpp"
#include "Player.hpp"
#include "Killable.hpp"
#include "Collider.hpp"
#include "Sounded.hpp"
#include "Clock.hpp"
#include "ShootingPoint.hpp"
#include "FreeSighted.hpp"
#include "AutoShooting.hpp"
#include "Drawable.hpp"
#include "Lifed.hpp"
#include "Scored.hpp"
#include "Animated.hpp"
#include "Shooting.hpp"
#include "conf/boss.hpp"
#include "shoot_utils.hpp"

using namespace lif::Conf::Boss::AlienBoss;
using lif::AlienBoss;

AlienBoss::AlienBoss(const sf::Vector2f& pos)
	: lif::Boss(pos)
{
	addComponent(new lif::Sounded(*this, {
		std::make_pair("death", lif::getAsset("test", std::string("alienboss_death.ogg"))),
		std::make_pair("hurt", lif::getAsset("test", std::string("alienboss_hurt.ogg")))
	}));
	const sf::Vector2f size(3 * lif::TILE_SIZE, 3 * lif::TILE_SIZE);
	collider = addComponent(new lif::Collider(*this, [this] (lif::Collider& coll) {
		// on collision
		_checkCollision(coll);
	}, lif::Layers::BOSSES, size));
	addComponent(new lif::Scored(*this, VALUE));
	animated = addComponent(new lif::Animated(*this, lif::getAsset("test", "alien_boss.png")));
	animated->addAnimation("idle", { sf::IntRect(0, 0, size.x, size.y) }, true);
	addComponent(new lif::Lifed(*this, LIFE));
	shootClock = addComponent(new lif::Clock(*this));

	lif::Attack attack;
	attack.type = lif::AttackType::SIMPLE;
	attack.bullet.id = 101;
	attack.bullet.damage = 4;
	attack.bullet.speed = 1.1;
	attack.fireRate = 1. / SHOOT_SHORT_INTERVAL.asSeconds();

	// Shooting points
	eyes[0] = addComponent(new lif::ShootingPoint(*this, attack, SIGHT_RADIUS));
	eyes[0]->setOffset(sf::Vector2f(17, 18));
	eyes[1] = addComponent(new lif::ShootingPoint(*this, attack, SIGHT_RADIUS));
	eyes[1]->setOffset(sf::Vector2f(50, 18));
	addComponent(new lif::AutoShooting(*eyes[0]));
	addComponent(new lif::AutoShooting(*eyes[1]));
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
