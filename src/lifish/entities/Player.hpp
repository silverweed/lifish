#pragma once

#include <array>
#include "Entity.hpp"
#include "game.hpp"
#include "conf/player.hpp"
#include "conf/bomb.hpp"

namespace lif {

class Bonusable;
class Killable;
class Clock;
class MovingAnimator;
class Clock;
class AxisMoving;
class Collider;
class Animated;
class RegularEntityDeath;

/** This structs contains all the data which is persistent through different levels. */
struct PlayerInfo {
	unsigned short id;
	struct {
		unsigned short bombRadius = lif::conf::bomb::DEFAULT_RADIUS;
		unsigned short maxBombs   = lif::conf::player::DEFAULT_MAX_BOMBS;
		sf::Time bombFuseTime     = lif::conf::bomb::DEFAULT_FUSE;
		bool incendiaryBomb       = false;
		bool throwableBomb        = false;
	} powers;

	short remainingLives = lif::conf::player::INITIAL_LIVES - 1;

	/** The EXTRA letters of this player */
	std::array<bool, lif::conf::player::N_EXTRA_LETTERS> extra;

	PlayerInfo(unsigned short id) : id(id) {
		extra.fill(false);
	}

	void reset(bool resetExtra) {
		if (resetExtra)
			extra.fill(false);
		powers.bombRadius = lif::conf::bomb::DEFAULT_RADIUS;
		powers.maxBombs = lif::conf::player::DEFAULT_MAX_BOMBS;
		powers.bombFuseTime = lif::conf::bomb::DEFAULT_FUSE;
		powers.incendiaryBomb = false;
		powers.throwableBomb = false;
	}
};

class Player;

class PlayerDrawProxy : public sf::Drawable {
	const lif::Player& player;
public:
	explicit PlayerDrawProxy(const lif::Player& player);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

/**
 * The player entity
 */
class Player : public lif::Entity {

	friend class lif::PlayerDrawProxy;

	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
	constexpr static unsigned short IDLE_N_FRAMES = 20;
	const static sf::Time DEATH_TIME;

	/** If true, idle animation is winning animation */
	bool winning = false;

	lif::AxisMoving *moving = nullptr;
	lif::Animated *animated = nullptr;
	lif::Bonusable *bonusable = nullptr;
	lif::MovingAnimator *movingAnimator = nullptr;
	lif::Killable *killable = nullptr;
	lif::RegularEntityDeath *death = nullptr;
	lif::Clock *hurtClock = nullptr;

	lif::PlayerInfo info;
	PlayerDrawProxy drawProxy;

	void _init();
	void _kill();
	void _hurt();
	void _checkCollision(lif::Collider& cld);

public:
	/** Creates a player with the default state and id `id` */
	explicit Player(const sf::Vector2f& pos, const unsigned short id);
	/** Creates a player whose state is described by `info` */
	explicit Player(const sf::Vector2f& pos, const lif::PlayerInfo& info);

	void resurrect();

	const lif::PlayerInfo& getInfo() const { return info; }
	void setBombRadius(unsigned short r) { info.powers.bombRadius = r; }
	void setMaxBombs(unsigned short m) { info.powers.maxBombs = m; }
	void setBombFuseTime(sf::Time t) { info.powers.bombFuseTime = t; }
	void setIncendiaryBomb(bool b) { info.powers.incendiaryBomb = b; }
	void setThrowableBomb(bool b) { info.powers.throwableBomb = b; }
	void setExtra(unsigned short n, bool e) { info.extra[n] = e; }
	void setRemainingLives(short l) { info.remainingLives = l; }

	void setWinning(bool b);

	void update() override;
};

}
