#pragma once

#include "Entity.hpp"
#include "Bomb.hpp"
#include "game.hpp"
#include "MovingAnimator.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"
#include "Animated.hpp"
#include "RegularEntityDeath.hpp"
#include "game_values.hpp"

namespace Game {

class Bonusable;

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
	std::array<bool, Game::Conf::Player::N_EXTRA_LETTERS> extra;

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

class Player;

class PlayerDrawProxy : public sf::Drawable {
	const Game::Player& player;
public:
	explicit PlayerDrawProxy(const Game::Player& player);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

/**
 * The player entity
 */
class Player : public Game::Entity {

	friend class Game::PlayerDrawProxy;

	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
	constexpr static unsigned short IDLE_N_FRAMES = 20;
	const static sf::Time DEATH_TIME;

	/** While true, the idle pose becomes ANIM_WIN */
	bool winning = false;
	short remainingLives = Game::Conf::Player::INITIAL_LIVES;

	Game::AxisMoving *moving = nullptr;
	Game::Animated *animated = nullptr;
	Game::Bonusable *bonusable = nullptr;
	Game::MovingAnimator *movingAnimator = nullptr;
	Game::Killable *killable = nullptr;
	Game::RegularEntityDeath *death = nullptr;
	Game::Clock *hurtClock = nullptr;

	Game::PlayerInfo info;
	PlayerDrawProxy drawProxy;

	void _init();
	void _kill();
	void _hurt();
	void _checkCollision(Game::Collider& cld);

public:
	/** Creates a player with the default state and id `id` */
	explicit Player(const sf::Vector2f& pos, const unsigned short id);
	/** Creates a player whose state is described by `info` */
	explicit Player(const sf::Vector2f& pos, const Game::PlayerInfo& info);

	void resurrect();
	short getRemainingLives() const { return remainingLives; }

	void setWinning(bool b) { winning = b; }

	const Game::PlayerInfo& getInfo() const { return info; }
	void setBombRadius(unsigned short r) { info.powers.bombRadius = r; }
	void setMaxBombs(unsigned short m) { info.powers.maxBombs = m; }
	void setBombFuseTime(sf::Time t) { info.powers.bombFuseTime = t; }
	void setExtra(unsigned short n, bool e) { info.extra[n] = e; }
	void setRemainingLives(short l) { info.remainingLives = l; }

	void update() override;
};

}
