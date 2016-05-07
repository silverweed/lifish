#pragma once

#include <SFML/System.hpp>
#include <memory>
#include "Entity.hpp"
#include "Clock.hpp"
#include "Animated.hpp"
#include "Direction.hpp"
#include "MovingAnimator.hpp"
//#include "AI.hpp"
#include "AlienSprite.hpp"
#include "Game.hpp"
#include "Shooting.hpp"

namespace Game {

class Enemy;

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

	std::unique_ptr<Game::EnemyDrawableProxy> drawProxy;

	Game::Clock<3> *clocks = nullptr;

	/** The function determining this enemy's movements */
	//AIBoundFunction ai;

	/** True when the enemy is shooting and atktype is BLOCKING */
	bool blocked = false;

	/** True when the enemy is morphed into a harmless Alien during EXTRA game */
	bool morphed = false;
	Game::AlienSprite *alienSprite = nullptr;

	/** Only used when this enemy's AI is ai_follow_dash: true while
	 *  the enemy is dashing towards the player
	 */
	bool dashing = false;
	const float originalSpeed;

public:
	constexpr static float BASE_SPEED = 75.f;

	unsigned short distanceWithNearestPlayer = 2 * Game::LEVEL_WIDTH * Game::TILE_SIZE;

	/** Whether this enemy is currently seeing the player (i.e.
	 *  it shares a coordinate with him and no walls are in the middle)
	 */
	Game::Direction seeingPlayer = Game::Direction::NONE;


	explicit Enemy(sf::Vector2f pos, unsigned short id, float speed, const Game::Attack& attack);

	//void setAI(AIFunction aifunc) { ai = aifunc(this); }
	//AIBoundFunction getAI() const { return ai; }

	/** Returns true if enemy has attacked and hasn't cooled down yet. */
	bool isRecharging() const;

	void block() { blocked = true; }
	bool isBlocked() const { return blocked; }

	void setMorphed(bool b);
	bool isMorphed() const { return morphed; }

	/** Returns true if enemy wasn't already dashing and now is, else false. */
	//bool setDashing(bool b);
	//bool isDashing() const { return dashing; }

	//void yell();

	void update() override;
};

}
