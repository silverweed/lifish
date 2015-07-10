#pragma once
/**
 * The player
 */

#include "MovingEntity.hpp"
#include "Bomb.hpp"
#include "Lifed.hpp"

namespace Game {

class Player : public Game::MovingEntity, public Game::Lifed {
	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
	constexpr static unsigned short DEFAULT_MAX_BOMBS = 5;

	unsigned short maxBombs = DEFAULT_MAX_BOMBS;

	bool _isTransparentTo(const Entity *const e) const override;
public:
	constexpr static unsigned short MAX_LIFE = 16;
	constexpr static unsigned short MAX_MAX_BOMBS = 8;

	struct {
		unsigned short bombRadius = Game::Bomb::DEFAULT_RADIUS;
		float bombFuseTime = Game::Bomb::DEFAULT_FUSE;
	} powers;

	Player(sf::Vector2f pos, const unsigned short id);

	virtual void move() override;
	void move(const Direction dir) override;

	unsigned short getMaxBombs() const { return maxBombs; }
};

}
