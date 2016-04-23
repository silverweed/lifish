#pragma once

#include "Entity.hpp"
#include "Bomb.hpp"
#include "Game.hpp"

namespace Game {

/**
 * The player
 */
class Player : public Game::Entity {
	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
	constexpr static unsigned short DEFAULT_MAX_BOMBS = 5;
	constexpr static float DEFAULT_SPEED = 150.f;
	constexpr static unsigned short DEATH_TIME = 5000; // ms

	/** The identifier of this Player */
	const unsigned short id;

	/** While true, the idle pose becomes ANIM_WIN */
	bool winning = false;


	void _kill(Game::Entity*);

public:
	constexpr static unsigned short MAX_LIFE = 16;
	constexpr static unsigned short MAX_MAX_BOMBS = 8;
	constexpr static unsigned short INITIAL_LIVES = 3;

	struct {
		unsigned short bombRadius = Game::Bomb::DEFAULT_RADIUS;
		/** In ms */
		unsigned short bombFuseTime = Game::Bomb::DEFAULT_FUSE;
		unsigned short maxBombs = DEFAULT_MAX_BOMBS;
	} powers;

	/** The EXTRA letters of this player */
	std::array<bool, Game::N_EXTRA_LETTERS> extra;

	explicit Player(const sf::Vector2f& pos, const unsigned short id);

	void resurrect();

	void setWinning(bool b) { winning = b; }
};

}
