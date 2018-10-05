#include "BigAlienBoss.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "BufferedSpawner.hpp"
#include "Clock.hpp"
#include "Drawable.hpp"
#include "Egg.hpp"
#include "GameCache.hpp"
#include "HurtDrawProxy.hpp"
#include "Killable.hpp"
#include "Lifed.hpp"
#include "MovingAnimator.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "ai_functions.hpp"
#include "conf/boss.hpp"
#include "conf/player.hpp"
#include "game.hpp"
#include <random>

using lif::BigAlienBoss;

static const sf::Vector2f SIZE { 5 * lif::TILE_SIZE, 5 * lif::TILE_SIZE };

BigAlienBoss::BigAlienBoss(const sf::Vector2f& pos, const lif::LevelManager& lm)
	: lif::Boss(pos)
	, lm(lm)
{
	moving = addComponent<lif::AxisMoving>(*this,
			lif::conf::boss::big_alien_boss::SPEED * lif::conf::player::DEFAULT_SPEED,
			lif::Direction::DOWN);
	addComponent<lif::AI>(*this, lif::ai_random_forward);
	addComponent<lif::Lifed>(*this, lif::conf::boss::big_alien_boss::LIFE);
	addComponent<lif::Scored>(*this, lif::conf::boss::big_alien_boss::VALUE);
	attackClock = addComponent<lif::Clock>(*this);
	addComponent<lif::Sounded>(*this,
		lif::sid("death"), lif::getAsset("sounds", "big_alien_boss_death.ogg"),
		lif::sid("hurt"), lif::getAsset("sounds", "big_alien_boss_hurt.ogg")
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
	addComponent<lif::MovingAnimator>(*this);
	spawner = addComponent<lif::BufferedSpawner>(*this);

	addComponent<lif::Drawable>(*this, *addComponent<lif::HurtDrawProxy>(*this));

	// Body collider
	_addDefaultCollider(SIZE);
}

void BigAlienBoss::update() {
	lif::Boss::update();

	if (killable->isKilled())
		return;

	if (attackClock->getElapsedTime() > lif::conf::boss::big_alien_boss::ATK_INTERVAL) {
		attackClock->restart();
		moving->block(sf::seconds(1));
		std::uniform_int_distribution<> dist(1, lif::N_ENEMIES);
		auto egg = new lif::Egg(position + eggOffset(),
				lif::oppositeDirection(moving->getDirection()), lm, dist(lif::rng));
		lif::cache.playSound(egg->get<lif::Sounded>()->getSoundFile("spawn"));
		spawner->addSpawned(egg);
	}
}

sf::Vector2f BigAlienBoss::eggOffset() const {
	switch (moving->getDirection()) {
		using D = lif::Direction;
	case D::UP:
		return sf::Vector2f(2 * lif::TILE_SIZE, 4.7 * lif::TILE_SIZE);
	case D::DOWN:
		return sf::Vector2f(2 * lif::TILE_SIZE, -0.7 * lif::TILE_SIZE);
	case D::RIGHT:
		return sf::Vector2f(-0.7 * lif::TILE_SIZE, 2 * lif::TILE_SIZE);
	case D::LEFT:
		return sf::Vector2f(4.7 * lif::TILE_SIZE, 2 * lif::TILE_SIZE);
	default:
		return sf::Vector2f();
	}
}

void BigAlienBoss::_kill() {
	lif::Boss::_kill();
	moving->setSpeed(0);
}
