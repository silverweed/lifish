#pragma once

#include <SFML/System.hpp>
#include "Entity.hpp"
#include "Clock.hpp"
#include "Direction.hpp"
//#include "AI.hpp"
#include "AlienSprite.hpp"

namespace Game {

/** 
 * An Enemy is a MovingEntity which (usually) shoots towards players 
 * when they see them and is vulnerable to Bombs.
 */
class Enemy : public Game::Entity {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static int YELL_DELAY = 1000;
	
	/** The duration of the shooting frame */
	sf::Time shootFrameTime = sf::milliseconds(250); // ms

	Game::Clock<3> *clocks = nullptr;

	/** The function determining this enemy's movements */
	//AIBoundFunction ai;

	bool shooting = false;

	/** True when the enemy is shooting and atktype is BLOCKING */
	bool blocked = false;

	/** True when the enemy is morphed into a harmless Alien during EXTRA game */
	bool morphed = false;
	Game::AlienSprite *alienSprite = nullptr;

	/** Only used when this enemy's AI is ai_follow_dash: true while
	 *  the enemy is dashing towards the player
	 */
	bool dashing = false;
	float originalSpeed;

public:
	constexpr static float BASE_SPEED = 75.f;

	unsigned short distanceWithNearestPlayer = 2 * Game::LEVEL_WIDTH * Game::TILE_SIZE;

	/** Whether this enemy is currently seeing the player (i.e.
	 *  it shares a coordinate with him and no walls are in the middle)
	 */
	Game::Direction seeingPlayer = Game::Direction::NONE;


	explicit Enemy(sf::Vector2f pos, const unsigned short id);

	//void setAI(AIFunction aifunc) { ai = aifunc(this); }
	//AIBoundFunction getAI() const { return ai; }

	/** Returns true if enemy has attacked and hasn't cooled down yet. */
	bool isRecharging() const;

	void shoot();
	bool isShooting() const { return shooting; }

	void block() { blocked = true; }
	bool isBlocked() const { return blocked; }

	void setMorphed(bool b);
	bool isMorphed() const { return morphed; }

	/** Returns true if enemy wasn't already dashing and now is, else false. */
	bool setDashing(bool b);
	bool isDashing() const { return dashing; }

	void yell();
};

}
