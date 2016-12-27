#pragma once

#include <memory>
#include <array>
#include <SFML/System.hpp>
#include "Entity.hpp"
#include "Attack.hpp"
#include "game.hpp"

namespace Game {

class Enemy;
class Collider;
class Animated;
class Clock;
class Killable;
class MovingAnimator;
class AI;
class AlienSprite;
class AxisSighted;
class AxisMoving;
class Shooting;
class AutoShooting;
class RegularEntityDeath;

struct EnemyInfo {
	unsigned short ai = 0;
	unsigned short speed = 0;
	Attack attack;	
};

/**
 * This class provides a Drawable proxy to draw the regular enemy's sprite
 * or its AlienSprite according to the morphed state of the Enemy.
 */
class EnemyDrawableProxy : public sf::Drawable {
	const Game::Enemy& enemy;
	const Game::Animated& morphedAnim;

public:
	explicit EnemyDrawableProxy(const Game::Enemy& e);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

/** 
 * An Enemy is a MovingEntity which (usually) shoots towards players 
 * when they see them and is vulnerable to Bombs.
 */
class Enemy : public Game::Entity {

	friend class EnemyDrawableProxy;

	std::unique_ptr<Game::EnemyDrawableProxy> drawProxy;

protected:
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static int YELL_DELAY = 1000;
	
	const unsigned short id;
	const Game::EnemyInfo info;

	Game::Shooting *shooting = nullptr;
	Game::AutoShooting *autoShooting = nullptr;
	Game::Collider *collider = nullptr;
	Game::Animated *animated = nullptr;
	Game::AxisMoving *moving = nullptr;
	Game::Killable *killable = nullptr;
	Game::MovingAnimator *movingAnimator = nullptr;
	Game::AxisSighted *sighted = nullptr;
	Game::AI *ai = nullptr;
	Game::RegularEntityDeath *death = nullptr;

	std::array<sf::Sprite, 4> shootFrame;

	Game::Clock *yellClock = nullptr,
		    *dashClock = nullptr;
	sf::Time morphDuration;

	/** True when the enemy is morphed into a harmless Alien during EXTRA game */
	bool morphed = false;
	Game::AlienSprite *alienSprite = nullptr;

	const float originalSpeed;


	virtual void _checkCollision(Game::Collider& coll);
	void _checkShoot();
	/** @return Whether entity `e` is within our Shooting range or not */
	bool _inRange(const Game::Entity *const e) const;

public:
	constexpr static float BASE_SPEED = 75.f;

	unsigned short distanceWithNearestPlayer = 2 * Game::LEVEL_WIDTH * Game::TILE_SIZE;

	// TODO: eliminate need for `id`
	explicit Enemy(sf::Vector2f pos, unsigned short id, const Game::EnemyInfo& info);

	void setMorphed(bool b);
	bool isMorphed() const { return morphed; }

	void update() override;
	void setOrigin(const sf::Vector2f& pos) override {
		Game::Entity::setOrigin(pos);
		for (auto& frame : shootFrame)
			frame.setOrigin(pos);
	}
};

}
