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
#include "boss.hpp"
#include "shoot_utils.hpp"

using namespace Game::Conf::Boss::AlienBoss;
using Game::AlienBoss;

AlienBoss::AlienBoss(const sf::Vector2f& pos)
	: Game::Boss(pos)
{
	addComponent(new Game::Sounded(*this, {
		Game::getAsset("test", std::string("alienboss_death.ogg")),
		Game::getAsset("test", std::string("alienboss_hurt.ogg"))
	}));
	const sf::Vector2i size(3 * Game::TILE_SIZE, 3 * Game::TILE_SIZE);
	collider = addComponent(new Game::Collider(*this, [this] (Game::Collider& coll) {
		// on collision
		_checkCollision(coll);
	}, Game::Layers::BOSSES, size));
	addComponent(new Game::Scored(*this, VALUE));
	animated = addComponent(new Game::Animated(*this, Game::getAsset("test", "alien_boss.png")));
	animated->addAnimation("idle", { sf::IntRect(0, 0, size.x, size.y) }, true);
	addComponent(new Game::Lifed(*this, LIFE));
	shootClock = addComponent(new Game::Clock(*this));

	Game::Attack attack;
	attack.type = Game::AttackType::SIMPLE;
	attack.bullet.id = 101;
	attack.bullet.damage = 4;
	attack.bullet.speed = 1.1;
	attack.fireRate = 1. / SHOOT_SHORT_INTERVAL.asSeconds();

	// Shooting points
	eyes[0] = addComponent(new Game::ShootingPoint(*this, attack, SIGHT_RADIUS));
	eyes[0]->setOffset(sf::Vector2f(17, 18));
	eyes[1] = addComponent(new Game::ShootingPoint(*this, attack, SIGHT_RADIUS));
	eyes[1]->setOffset(sf::Vector2f(50, 18));
	addComponent(new Game::AutoShooting(*eyes[0]));
	addComponent(new Game::AutoShooting(*eyes[1]));
}

void AlienBoss::update() {
	Game::Entity::update();
	if (killable->isKilled() || eyes[0]->get<Game::Shooting>()->isRecharging()) return;
	
	if (shotsFired > 0 || shootClock->getElapsedTime() > SHOOT_INTERVAL) {
		for (auto eye : eyes)
			Game::shootToNearestPlayer(*eye);
		shotsFired = (shotsFired + 1) % N_SHOTS;
		if (shotsFired == 0)
			shootClock->restart();
	}
}
