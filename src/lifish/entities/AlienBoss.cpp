#include "AlienBoss.hpp"
#include "Player.hpp"
#include "Collider.hpp"
#include "Sounded.hpp"
#include "Drawable.hpp"
#include "Lifed.hpp"
#include "Scored.hpp"
#include "Sprite.hpp"

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
		// TODO
		return false;
	}));
	addComponent(new Game::Scored(*this, VALUE));
	addComponent(new Game::Drawable(*this, *addComponent(new Game::Sprite(*this, 
				Game::getAsset("test", "alien_boss.png"), sf::IntRect(0, 0, SIZE.x, SIZE.y)))));
	addComponent(new Game::Lifed(*this, LIFE));

	Game::Attack attack;
	attack.type = Game::AttackType::SIMPLE;
	attack.id = 101;
	attack.damage = 4;
	attack.speed = 160;
	attack.fireRate = 1. / SHOOT_SHORT_INTERVAL.asSeconds();

	shooting = addComponent(new Game::Shooting(*this, attack));
	autoShooting = addComponent(new Game::AutoShooting(*this));
	shootClock = addComponent(new Game::Clock(*this));

	// Shooting points
	eyes[0] = addComponent(new Game::FreeSighted(*this, SIGHT_RADIUS));
	eyes[0]->setPosition(position + sf::Vector2f(34, 36));
	eyes[1] = addComponent(new Game::FreeSighted(*this, SIGHT_RADIUS));
	eyes[1]->setPosition(position + sf::Vector2f(63, 36));
}

void AlienBoss::_kill() {
	// TODO
}

void AlienBoss::update() {
	Game::Boss::update();
	if (killable->isKilled() || shooting->isRecharging()) return;
	
	if (shotsFired > 0 || shootClock->getElapsedTime() > SHOOT_INTERVAL) {
		_shoot();
		shotsFired = (shotsFired + 1) % N_SHOTS;
		if (shotsFired == 0)
			shootClock->restart();
	}
}

void AlienBoss::_shoot() {
	for (auto eye : eyes) {
		const auto player = eye->nearest<Game::Player>();
		if (player == nullptr) return;

		const auto& ppos = player->getPosition();

		// calculate angle with ppos: a = pi - arctan(dy / dx)
		const double dx = eye->getPosition().x - ppos.x,
			     dy = ppos.y - eye->getPosition().y,
			     angle = Game::PI - std::atan2(dy, dx);

		autoShooting->shoot(angle);
		// TODO
		//auto bullet = new Game::BossBullet(eye, angle);
		//bullet->setOrigin(origin);
		//bullet->setSource(boss);
		//bullets.push_back(bullet);
		//Game::cache.playSound(bullet->getSoundFile(Game::Sounds::SHOT));
	}
}
