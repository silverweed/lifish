#pragma once

#include <SFML/System.hpp>
#include <memory>
#include "Entity.hpp"
#include "Clock.hpp"
#include "Animated.hpp"
#include "Direction.hpp"
#include "MovingAnimator.hpp"
#include "AI.hpp"
#include "AlienSprite.hpp"
#include "Sighted.hpp"
#include "Game.hpp"
#include "Shooting.hpp"

namespace Game {

class Enemy;

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
	Game::Enemy& enemy;
	Game::Animated *enemyAnim = nullptr,
		       *morphedAnim = nullptr;

public:
	explicit EnemyDrawableProxy(Enemy& e);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

/** 
 * An Enemy is a MovingEntity which (usually) shoots towards players 
 * when they see them and is vulnerable to Bombs.
 */
class Enemy : public Game::Entity {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static int YELL_DELAY = 1000;
	
	/** The duration of the shooting frame */
	sf::Time shootFrameTime = sf::milliseconds(250); // ms

	Game::Shooting *shooting = nullptr;
	Game::Animated *animated = nullptr;
	Game::AxisMoving *moving = nullptr;
	Game::MovingAnimator *movingAnimator = nullptr;
	Game::Sighted *sighted = nullptr;
	Game::AI *ai = nullptr;

	std::unique_ptr<Game::EnemyDrawableProxy> drawProxy;

	Game::Clock *attackClock = nullptr,
		    *yellClock = nullptr,
		    *dashClock = nullptr;

	/** The function determining this enemy's movements */
	//AIBoundFunction ai;

	/** True when the enemy is shooting and atktype is BLOCKING */
	bool blocked = false;

	/** True when the enemy is morphed into a harmless Alien during EXTRA game */
	bool morphed = false;
	Game::AlienSprite *alienSprite = nullptr;

	const float originalSpeed;


	void _checkCollision(Game::Collider& coll);

public:
	constexpr static float BASE_SPEED = 75.f;

	unsigned short distanceWithNearestPlayer = 2 * Game::LEVEL_WIDTH * Game::TILE_SIZE;

	explicit Enemy(sf::Vector2f pos, unsigned short id, const Game::EnemyInfo& info);

	//void setAI(AIFunction aifunc) { ai = aifunc(this); }
	//AIBoundFunction getAI() const { return ai; }

	void block() { blocked = true; }
	bool isBlocked() const { return blocked; }

	void setMorphed(bool b);
	bool isMorphed() const { return morphed; }

	void update() override;
};

}
