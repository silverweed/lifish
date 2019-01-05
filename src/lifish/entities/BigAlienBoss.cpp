#include "BigAlienBoss.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "Bonusable.hpp"
#include "BufferedSpawner.hpp"
#include "Drawable.hpp"
#include "Egg.hpp"
#include "Enemy.hpp"
#include "EnergyBar.hpp"
#include "Explosion.hpp"
#include "GameCache.hpp"
#include "HurtDrawProxy.hpp"
#include "Killable.hpp"
#include "Level.hpp"
#include "LevelManager.hpp"
#include "Lifed.hpp"
#include "MovingAnimator.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "Sprite.hpp"
#include "Time.hpp"
#include "ai_functions.hpp"
#include "conf/boss.hpp"
#include "conf/player.hpp"
#include "game.hpp"
#include <random>
#ifndef RELEASE
	#include "DebugPainter.hpp"
#endif

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
	addComponent<lif::Lifed>(*this, lif::conf::boss::big_alien_boss::LIFE, [this] (int, int newLife) {
		energyBar->setEnergy(newLife);
	});
	addComponent<lif::Scored>(*this, lif::conf::boss::big_alien_boss::VALUE);
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

	energyBar = addComponent<lif::EnergyBar>(*this);
	energyBar->setPosition(lif::center(
		energyBar->get<lif::Sprite>()->getSprite().getLocalBounds(),
		sf::FloatRect(0, 0, (lm.getLevel()->getInfo().width + 2) * lif::TILE_SIZE, 22)));

	drawable = addComponent<lif::Drawable>(*this, *addComponent<lif::HurtDrawProxy>(*this));;

	// Body collider
	collider = addComponent<lif::Collider>(*this, [this] (lif::Collider& coll) {
		// on collision
		if (!(killable && killable->isKilled()))
			_checkCollision(coll);
	}, lif::c_layers::BOSSES, SIZE);
}

void BigAlienBoss::update() {
	lif::Boss::update();

	if (killable->isKilled())
		return;

	atkT += lif::time.getDelta();

	if (atkT > lif::conf::boss::big_alien_boss::ATK_INTERVAL) {
		atkT = sf::Time::Zero;
		moving->block(sf::seconds(1));
		std::uniform_int_distribution<> dist(1, lif::N_ENEMIES);
		auto egg = new lif::Egg(position + _eggOffset(),
				lif::oppositeDirection(moving->getDirection()), lm, dist(lif::rng));
		lif::cache.playSound(egg->get<lif::Sounded>()->getSoundFile("spawn"));
		spawner->addSpawned(egg);
	}
}

sf::Vector2f BigAlienBoss::_eggOffset() const {
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

	energyBar->moveOut();

	// Kill every enemy on screen
	lm.getEntities().apply([] (auto entity) {
		auto enemy = dynamic_cast<lif::Enemy*>(&entity);
		if (enemy == nullptr) return;
		enemy->get<lif::Killable>()->kill();
	});
}

// This is like Boss::_checkCollision but each bomb only inflicts 1 damage
void BigAlienBoss::_checkCollision(lif::Collider& coll) {
	// One-shot players
	if (coll.getLayer() == lif::c_layers::PLAYERS) {
		auto& player = static_cast<lif::Player&>(coll.getOwnerRW());
		if (!player.get<lif::Bonusable>()->hasBonus(lif::BonusType::SHIELD)) {
			player.dealDamage(lif::conf::player::MAX_LIFE, true);
		}
		return;
	}

	if (!vulnerable || coll.getLayer() != lif::c_layers::EXPLOSIONS) return;

	auto& expl = static_cast<lif::Explosion&>(coll.getOwnerRW());
	if (expl.hasDamaged(*this)) return;

	const auto damage = 1;
	if (get<lif::Lifed>()->decLife(damage) > 0)
		get<lif::HurtDrawProxy>()->hurt();
	else
		killable->kill();
	expl.dealDamageTo(*this);
	lif::cache.playSound(get<lif::Sounded>()->getSoundFile("hurt"));
}
