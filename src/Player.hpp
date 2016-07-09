#pragma once

#include "Entity.hpp"
#include "Bomb.hpp"
#include "Game.hpp"
#include "MovingAnimator.hpp"
#include "AxisMoving.hpp"
#include "Animated.hpp"
#include "game_values.hpp"

namespace Game {

/** This structs contains all the data which is persistent through different levels. */
struct PlayerInfo {
	unsigned short id;
	struct {
		unsigned short bombRadius = Game::Conf::Bomb::DEFAULT_RADIUS;
		unsigned short maxBombs   = Game::Conf::Player::DEFAULT_MAX_BOMBS;
		sf::Time bombFuseTime     = Game::Conf::Bomb::DEFAULT_FUSE;
	} powers;

	short remainingLives = Game::Conf::Player::INITIAL_LIVES;

	/** The EXTRA letters of this player */
	std::array<bool, Game::N_EXTRA_LETTERS> extra;

	PlayerInfo(unsigned short id) : id(id) {
		extra.fill(false);
	}

	void reset() {
		extra.fill(false);
		powers.bombRadius = Game::Conf::Bomb::DEFAULT_RADIUS;
		powers.maxBombs = Game::Conf::Player::DEFAULT_MAX_BOMBS;
		powers.bombFuseTime = Game::Conf::Bomb::DEFAULT_FUSE;
	}
};

/**
 * The player entity
 */
class Player : public Game::Entity {
private:
	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
	const static sf::Time DEATH_TIME;

	/** While true, the idle pose becomes ANIM_WIN */
	bool winning = false;
	short remainingLives = Game::Conf::Player::INITIAL_LIVES;

	Game::AxisMoving *moving = nullptr;
	Game::Animated *animated = nullptr;
	Game::MovingAnimator *movingAnimator = nullptr;

	Game::PlayerInfo info;


	void _init();
	void _kill();

public:
	/** Creates a player with the default state and id `id` */
	explicit Player(const sf::Vector2f& pos, const unsigned short id);
	/** Creates a player whose state is described by `info` */
	explicit Player(const sf::Vector2f& pos, const Game::PlayerInfo& info);

	void resurrect();
	short getRemainingLives() const { return remainingLives; }

	void setWinning(bool b) { winning = b; }

	const Game::PlayerInfo& getInfo() const { return info; }
};

}
