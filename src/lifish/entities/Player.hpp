#pragma once

#include "Entity.hpp"
#include "conf/bomb.hpp"
#include "conf/player.hpp"
#include "game.hpp"
#include <array>

namespace lif {

class Bonusable;
class Killable;
class MovingAnimator;
class AxisMoving;
class Collider;
class Animated;
class RegularEntityDeath;

struct PlayerPowers {
	int bombRadius        = lif::conf::bomb::DEFAULT_RADIUS;
	int maxBombs          = lif::conf::player::DEFAULT_MAX_BOMBS;
	short absorbKillCount = 0;
	sf::Time bombFuseTime = lif::conf::bomb::DEFAULT_FUSE;
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
	}
};

class Player;

class PlayerDrawProxy : public sf::Drawable {
	const lif::Player& player;
	sf::Shader shieldShader;

public:
	explicit PlayerDrawProxy(const lif::Player& player);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

/**
 * The player entity
 */
class Player : public lif::Entity {

	friend class lif::PlayerDrawProxy;

	bool isHurt = false;
	bool winning = false;

	lif::AxisMoving *moving = nullptr;
	lif::Animated *animated = nullptr;
	lif::Bonusable *bonusable = nullptr;
	lif::MovingAnimator *movingAnimator = nullptr;
	lif::Killable *killable = nullptr;
	lif::RegularEntityDeath *death = nullptr;
	lif::Collider *collider = nullptr;

	sf::Time hurtT;

	lif::PlayerInfo info;
	PlayerDrawProxy drawProxy;

	void _init();
	void _setupAnimations();
	void _kill();
	void _hurt();
	void _checkCollision(lif::Collider& cld);
	std::string _getDirectionString() const;
	bool _isIdleAnim() const;

public:
	/** Creates a player with the default state and id `id` */
	explicit Player(const sf::Vector2f& pos, int id);
	/** Creates a player whose state is described by `info` */
	explicit Player(const sf::Vector2f& pos, const lif::PlayerInfo& info);

	void resurrect();

	const lif::PlayerInfo& getInfo() const { return info; }
	lif::PlayerPowers& getPowers() { return info.powers; }
	void setExtra(int n, bool e);
	void setRemainingLives(int l) { info.remainingLives = l; }

	void setWinning(bool b);

	void update() override;

	/** Convenient function to apply damage to the player, optionally considering its armor.
	 *  This function also checks if player's HP becomes 0 and kills it in that case.
	 *  This function DOES NOT CHECK if the player has shield.
	 */
	void dealDamage(int dmg, bool giveShortShield = false);
};

}
