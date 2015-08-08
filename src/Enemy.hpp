#pragma once

#include "LifedMovingEntity.hpp"
#include "AI.hpp"
#include "Scored.hpp"
#include "AlienSprite.hpp"

namespace Game {

/** 
 * An Enemy is a MovingEntity which (usually) shoots towards players 
 * when they see them and is vulnerable to Bombs.
 */
class Enemy : public Game::LifedMovingEntity, public Game::Scored {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	
	/** Indexed with ANIM_UP etc */
	sf::Sprite shootFrame[4];
	/** The duration of the shooting frame */
	unsigned short shootFrameTime = 250; // ms

	/** The function determining this enemy's movements */
	AIBoundFunction ai;

	sf::Clock attackClock;
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
	/** Used to keep a cooldown between two following dashes */
	sf::Clock dashClock;
	float originalSpeed;

	bool _isTransparentTo(const Entity *const e) const override;
public:
	constexpr static float BASE_SPEED = 75.f;
	enum AttackType : unsigned short {
		SIMPLE   = 1,
		CONTACT  = 1 << 1,
		RANGED   = 1 << 2,
		BLOCKING = 1 << 3
	};

	struct {
		unsigned short type;
		/** Cooldown is 1000/fireRate ms. If AI is ai_follow_dash, determines
		 *  the cooldown between two dashes.
		 */
		float fireRate;
		/** If attacktype is SIMPLE & BLOCKING, this is the time
		 *  the enemy stops after shooting (in ms -- should be more than shootFrameTime);
		 */
		unsigned short blockTime;
		unsigned short id;
		unsigned short damage;
		/** Projectile speed, in units of Bullet::BASE_SPEED */
		float speed;
		short range;
	} attack;
	/** Used by CONTACT attack AI */
	sf::Vector2i attackAlign;

	unsigned short distanceWithNearestPlayer = 2 * Game::LEVEL_WIDTH * Game::TILE_SIZE;

	/** Whether this enemy is currently seeing the player (i.e.
	 *  it shares a coordinate with him and no walls are in the middle)
	 */
	Game::Direction seeingPlayer = Game::Direction::NONE;


	Enemy(sf::Vector2f pos, const unsigned short id);
	~Enemy();

	void setAI(AIFunction aifunc) { ai = aifunc(this); }
	AIBoundFunction getAI() const { return ai; }

	void stop() override { frameClock.restart(); }

	/** Returns true if enemy has attacked and hasn't cooled down yet. */
	bool isRecharging() const;

	void shoot();
	bool isShooting() const { return shooting; }

	void draw(sf::RenderTarget& window) override;

	void move(const Game::Direction dir) override;

	void setOrigin(const sf::Vector2f& pos) override {
		for (unsigned short i = 0; i < 4; ++i)
			shootFrame[i].setOrigin(pos);
		Game::MovingEntity::setOrigin(pos);
	}

	void block() { blocked = true; }
	bool isBlocked() const { return blocked; }

	void setMorphed(bool b);
	bool isMorphed() const { return morphed; }

	void setDashing(bool b);
	bool isDashing() const { return dashing; }
};

}
