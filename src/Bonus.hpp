#pragma once

#include <random>
#include "Scored.hpp"
#include "Sounded.hpp"
#include "NonAnimated.hpp"
#include "Clocked.hpp"

namespace Game {

/**
 * The bonuses dropped by walls
 */
class Bonus 
	: public Game::NonAnimated
	, public Game::Scored
	, public Game::Sounded
	, public Game::Clocked
{
	constexpr static float EXPIRE_TIME = 10; // s
	constexpr static int VALUE = 100; // FIXME

	const unsigned short type;

	sftools::Chronometer expireClock;
public:
	constexpr static unsigned short N_BONUS_TYPES = 9;
	constexpr static unsigned short N_PERMANENT_BONUS_TYPES = 5;
	constexpr static unsigned short SHIELD_DURATION = 20000; // ms
	constexpr static unsigned short SPEEDY_DURATION = 20000; // ms

	enum Type : unsigned short {
		// "Permanent" bonuses
		MAX_BOMBS    = 0,
		QUICK_FUSE   = 1,
		MAX_RANGE    = 2,
		SHIELD       = 3,
		SPEEDY       = 4,
		// One-time bonuses
		ZAPPER       = 5,
		SUDDEN_DEATH = 6,
		HEALTH_SMALL = 7,
		HEALTH_FULL  = 8
	};

	Bonus(const sf::Vector2f& pos, const unsigned short type);

	unsigned short getType() const { return type; }

	bool isExpired() const { return expireClock.getElapsedTime().asSeconds() >= EXPIRE_TIME; }

	void draw(sf::RenderTarget& window) override;
};

extern std::discrete_distribution<unsigned short> bonusTypeDistribution;
}
