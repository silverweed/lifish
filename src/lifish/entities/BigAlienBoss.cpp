#include "BigAlienBoss.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "BufferedSpawner.hpp"
#include "Clock.hpp"
#include "Drawable.hpp"
#include "Egg.hpp"
#include "HurtDrawProxy.hpp"
#include "Lifed.hpp"
#include "MovingAnimator.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "conf/boss.hpp"
#include "conf/player.hpp"
#include "game.hpp"

using lif::BigAlienBoss;

static const sf::Vector2f SIZE { 5 * lif::TILE_SIZE, 5 * lif::TILE_SIZE };

BigAlienBoss::BigAlienBoss(const sf::Vector2f& pos, const lif::LevelManager& lm)
	: lif::Boss(pos)
	, lm(lm)
{
	moving = addComponent<lif::AxisMoving>(*this,
			lif::conf::boss::big_alien_boss::SPEED * lif::conf::player::DEFAULT_SPEED,
			lif::Direction::NONE);
	//addComponent<lif::AI>(*this, ai_rex);
	addComponent<lif::Lifed>(*this, lif::conf::boss::big_alien_boss::LIFE);
	addComponent<lif::Scored>(*this, lif::conf::boss::big_alien_boss::VALUE);
	attackClock = addComponent<lif::Clock>(*this);
	addComponent<lif::Sounded>(*this,
		lif::sid("death"), lif::getAsset("sounds", "rex_death.ogg"),
		lif::sid("hurt"), lif::getAsset("sounds", "rex_hurt.ogg")
	);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "big_alien_boss.png"));
	animated->addAnimation("walk_down", {
		sf::IntRect(0 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(2 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(3 * SIZE.x, 0 * SIZE.y, SIZE.x, SIZE.y),
	});
	animated->addAnimation("walk_up", {
		sf::IntRect(0 * SIZE.x, 1 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 1 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(2 * SIZE.x, 1 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(3 * SIZE.x, 1 * SIZE.y, SIZE.x, SIZE.y),
	});
	animated->addAnimation("walk_right", {
		sf::IntRect(0 * SIZE.x, 2 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 2 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(2 * SIZE.x, 2 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(3 * SIZE.x, 2 * SIZE.y, SIZE.x, SIZE.y),
	});
	animated->addAnimation("walk_left", {
		sf::IntRect(0 * SIZE.x, 3 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(1 * SIZE.x, 3 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(2 * SIZE.x, 3 * SIZE.y, SIZE.x, SIZE.y),
		sf::IntRect(3 * SIZE.x, 3 * SIZE.y, SIZE.x, SIZE.y),
	});
	animated->setDefaultFrameTime(sf::seconds(0.12));
	animated->setAnimation("walk_left");
	animated->getSprite().setLooped(true);
	animated->getSprite().play();
	addComponent<lif::MovingAnimator>(*this)->setActive(false);
	spawner = addComponent<lif::BufferedSpawner>(*this);

	addComponent<lif::Drawable>(*this, *addComponent<lif::HurtDrawProxy>(*this));

	// Body collider
	_addDefaultCollider(SIZE);
}

void BigAlienBoss::update() {
	lif::Boss::update();

	if (attackClock->getElapsedTime() > lif::conf::boss::big_alien_boss::ATK_INTERVAL) {
		attackClock->restart();
		spawner->addSpawned(new lif::Egg(position +
			sf::Vector2f(4 * lif::TILE_SIZE, 2 * lif::TILE_SIZE), lif::Direction::RIGHT, lm, 1));
	}
}
