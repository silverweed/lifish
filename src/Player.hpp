#pragma once

#include "Entity.hpp"
#include "Bomb.hpp"
#include "Game.hpp"

namespace Game {

/**
 * The player
 */
class Player : public Game::Entity {
public:
	constexpr static unsigned short MAX_LIFE = 16;
	constexpr static unsigned short MAX_MAX_BOMBS = 8;
	constexpr static unsigned short INITIAL_LIVES = 3;

private:
	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
	constexpr static unsigned short DEFAULT_MAX_BOMBS = 5;
	constexpr static float DEFAULT_SPEED = 150.f;
	const static sf::Time DEATH_TIME;

	/** The identifier of this Player */
	const unsigned short id;

	/** While true, the idle pose becomes ANIM_WIN */
	bool winning = false;

	short remainingLives = INITIAL_LIVES;


	void _kill();

public:
	struct {
		unsigned short bombRadius = Game::Bomb::DEFAULT_RADIUS;
		sf::Time bombFuseTime     = Game::Bomb::DEFAULT_FUSE;
		unsigned short maxBombs   = DEFAULT_MAX_BOMBS;
	} powers;

	/** The EXTRA letters of this player */
	std::array<bool, Game::N_EXTRA_LETTERS> extra;

	explicit Player(const sf::Vector2f& pos, const unsigned short id);

	void resurrect();

	void setWinning(bool b) { winning = b; }
};

}
