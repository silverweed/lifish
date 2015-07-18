#pragma once

#include "Lifed.hpp"
#include "MovingEntity.hpp"
#include "AI.hpp"
#include "Scored.hpp"

namespace Game {

class Enemy : public Game::MovingEntity, public Game::Lifed, public Game::Scored {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	
	/** Indexed with ANIM_UP etc */
	sf::Sprite shootFrame[4];

	/** The function determining this enemy's movements */
	AIBoundFunction ai;

	bool _isTransparentTo(const Entity *const e) const override;
public:
	constexpr static float BASE_SPEED = 75.f;
	enum AttackType : unsigned short {
		SIMPLE      = 1,
		CONTACT     = 1 << 1,
		CONTINUOUS  = 1 << 2,
		RANGED      = 1 << 3,
		BLOCKING    = 1 << 4
	};

	struct {
		unsigned short type;
		/** If atktype != CONTINUOUS, cooldown is 1000/fireRate ms. */
		float fireRate;
		unsigned short id;
		unsigned short damage;
		/** Projectile speed, in units of Bullet::BASE_SPEED */
		float speed;
		short range;
	} attack;	

	/** Whether this enemy is currently seeing the player (i.e.
	 *  it shares a coordinate with him and no walls are in the middle)
	 */
	Game::Direction seeingPlayer = Game::Direction::NONE;

	Enemy(sf::Vector2f pos, const unsigned short id);

	void setAI(AIFunction aifunc) { ai = aifunc(this); }
	AIBoundFunction getAI() const { return ai; }
};

}
