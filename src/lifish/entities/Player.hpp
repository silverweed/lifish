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

struct PlayerPowers {
	int bombRadius        = lif::conf::bomb::DEFAULT_RADIUS;
	int maxBombs          = lif::conf::player::DEFAULT_MAX_BOMBS;
	/** Every kill heals the player by `absorb` hp. */
	int absorb            = 0;
	/** Reduce every damage by `armor` (never below 1) */
	int armor             = 0;
	sf::Time bombFuseTime = lif::conf::bomb::DEFAULT_FUSE;
	bool incendiaryBomb   = false;
	bool throwableBomb    = false;
};

/** This structs contains all the data which is persistent through different levels. */
struct PlayerInfo {
	int id;

	lif::PlayerPowers powers;

	short remainingLives = lif::conf::player::INITIAL_LIVES - 1;

	/** The EXTRA letters of this player */
	std::array<bool, lif::conf::player::N_EXTRA_LETTERS> extra;

	PlayerInfo(int id) : id(id) {
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
		powers.absorb = 0;
		powers.armor = 0;
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

	constexpr static int WALK_N_FRAMES = 8;
	constexpr static int DEATH_N_FRAMES = 3;
	constexpr static int IDLE_N_FRAMES = 20;
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
	explicit Player(const sf::Vector2f& pos, const int id);
	/** Creates a player whose state is described by `info` */
	explicit Player(const sf::Vector2f& pos, const lif::PlayerInfo& info);

	void resurrect();

	const lif::PlayerInfo& getInfo() const { return info; }
	lif::PlayerPowers& getPowers() { return info.powers; }
	void setExtra(int n, bool e) { info.extra[n] = e; }
	void setRemainingLives(int l) { info.remainingLives = l; }

	void setWinning(bool b);

	void update() override;
};

}
