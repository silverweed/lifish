#pragma once

#include "Lifed.hpp"
#include "MovingEntity.hpp"
#include "AI.hpp"

namespace Game {

class Enemy : public Game::MovingEntity, public Game::Lifed {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	
	/** Indexed with ANIM_UP etc */
	sf::Sprite shootFrame[4];

	/** The function determining this enemy's movements */
	AIBoundFunction ai;

	// TODO: type of bullets, fire rate, etc
	bool _isTransparentTo(const Entity *const e) const override;
public:
	constexpr static float BASE_SPEED = 75.f;

	/** Whether this enemy is currently seeing the player (i.e.
	 *  it shares a coordinate with him and no walls are in the middle)
	 */
	Game::Direction seeingPlayer = Game::Direction::NONE;

	Enemy(sf::Vector2f pos, const unsigned short id);

	void setAI(AIFunction aifunc) { ai = aifunc(this); }
	AIBoundFunction getAI() const { return ai; }
};

}
