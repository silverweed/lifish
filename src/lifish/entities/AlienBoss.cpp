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
#include "Sprite.hpp"
#include "Shooting.hpp"

using namespace Game::Conf::Boss::AlienBoss;
using Game::AlienBoss;
 
const sf::Vector2i AlienBoss::SIZE = sf::Vector2i(3 * Game::TILE_SIZE, 3 * Game::TILE_SIZE);

AlienBoss::AlienBoss(const sf::Vector2f& pos)
	: Game::Boss(pos)
{
	addComponent(new Game::Sounded(*this, {
		Game::getAsset("test", std::string("alienboss_death.ogg")),
		Game::getAsset("test", std::string("alienboss_hurt.ogg"))
	}));
	collider = addComponent(new Game::Collider(*this, [this] (Game::Collider& coll) {
		// on collision
		_checkCollision(coll);
	}, Game::Layers::BOSSES, SIZE));
	killable = addComponent(new Game::Killable(*this, [this] () {
		// on kill
		_kill();
	}, [this] () {
		// kill in progress
		return _killInProgress();
	}));
	addComponent(new Game::Scored(*this, VALUE));
	addComponent(new Game::Drawable(*this, *addComponent(new Game::Sprite(*this, 
				Game::getAsset("test", "alien_boss.png"), sf::IntRect(0, 0, SIZE.x, SIZE.y)))));
	addComponent(new Game::Lifed(*this, LIFE));
	shootClock = addComponent(new Game::Clock(*this));

	Game::Attack attack;
	attack.type = Game::AttackType::SIMPLE;
	attack.id = 101;
	attack.damage = 4;
	attack.speed = 160;
	attack.fireRate = 1. / SHOOT_SHORT_INTERVAL.asSeconds();

	// Shooting points
	eyes[0] = addComponent(new Game::ShootingPoint(*this, attack, SIGHT_RADIUS));
	eyes[0]->setOffset(sf::Vector2f(17, 18));
	eyes[1] = addComponent(new Game::ShootingPoint(*this, attack, SIGHT_RADIUS));
	eyes[1]->setOffset(sf::Vector2f(50, 18));
}

void AlienBoss::_kill() {
	Boss::_kill();
	// TODO
}

void AlienBoss::update() {
	Game::Boss::update();
	if (killable->isKilled() || eyes[0]->get<Game::Shooting>()->isRecharging()) return;
	
	if (shotsFired > 0 || shootClock->getElapsedTime() > SHOOT_INTERVAL) {
		_shoot();
		shotsFired = (shotsFired + 1) % N_SHOTS;
		if (shotsFired == 0)
			shootClock->restart();
	}
}

void AlienBoss::_shoot() {
	int i = 0;
	for (auto eye : eyes) {
		++i;
		const auto player = eye->get<Game::FreeSighted>()->nearest<Game::Player>();
		if (player == nullptr) continue;

		const auto& ppos = player->getPosition();

		// calculate angle with ppos: a = pi - arctan(dy / dx)
		const double dx = eye->getPosition().x - ppos.x,
			     dy = ppos.y - eye->getPosition().y,
			     angle = Game::PI - std::atan2(dy, dx);

		eye->get<Game::AutoShooting>()->shoot(angle);
	}
}
