#pragma once

#include "Lifed.hpp"
#include "MovingEntity.hpp"
#include "AI.hpp"

namespace Game {

class Enemy : public Game::MovingEntity, public Game::Lifed {
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static unsigned short DEATH_N_FRAMES = 2;
	
	/** Indexed with ANIM_UP etc */
	sf::Sprite shootFrame[4];

	/** The function determining this enemy's movements */
	AIBoundFunction ai;

	// TODO: type of bullets, fire rate, etc
	bool _isTransparentTo(const Entity *const e) const override;
public:
	unsigned short steps = 0;

	Enemy(sf::Vector2f pos, const unsigned short id);

	void setAI(AIFunction aifunc) { ai = aifunc(this); }
	AIBoundFunction getAI() const { return ai; }
};

}
