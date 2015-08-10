#pragma once

#include "LifedMovingEntity.hpp"
#include "Bomb.hpp"
#include "Game.hpp"

namespace Game {

/**
 * The player
 */
class Player : public Game::LifedMovingEntity {
	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
	constexpr static unsigned short DEFAULT_MAX_BOMBS = 5;

	constexpr static unsigned short DEATH_TIME = 5000; // ms

	/** If true, the idle pose becomes ANIM_WIN */
	bool winMode = false;

	bool _isTransparentTo(const Entity *const e) const override;
public:
	constexpr static unsigned short MAX_LIFE = 16;
	constexpr static unsigned short MAX_MAX_BOMBS = 8;

	struct {
		unsigned short bombRadius = Game::Bomb::DEFAULT_RADIUS;
		/** In ms */
		unsigned short bombFuseTime = Game::Bomb::DEFAULT_FUSE;
		unsigned short maxBombs = DEFAULT_MAX_BOMBS;
	} powers;

	/** The EXTRA letters of this player */
	std::array<bool, Game::N_EXTRA_LETTERS> extra;

	Player(sf::Vector2f pos, const unsigned short id);

	virtual void move() override;
	void move(const Direction dir) override;

	void stop() override;

	void kill() override;
	void resurrect();

	bool playDeathAnimation() override;

	void setWinMode(bool b) { winMode = b; }
};

}
