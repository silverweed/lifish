#pragma once
/**
 * The bonuses dropped by walls
 */

#include "FixedEntity.hpp"
#include "Scored.hpp"
#include <random>

namespace Game {

class Bonus : public Game::FixedEntity, public Game::Scored {
	constexpr static float EXPIRE_TIME = 10; // s
	constexpr static int VALUE = 100; // FIXME

	const unsigned short type;

	sf::Clock expireClock;
public:
	constexpr static unsigned short N_BONUS_TYPES = 9;
	constexpr static unsigned short SHIELD_DURATION = 20000; // ms
	constexpr static unsigned short SPEEDY_DURATION = 20000; // ms

	enum Type : unsigned short {
		ZAPPER       = 0,
		SUDDEN_DEATH = 1,
		MAX_BOMBS    = 2,
		QUICK_FUSE   = 3,
		MAX_RANGE    = 4,
		HEALTH_SMALL = 5,
		HEALTH_FULL  = 6,
		SHIELD       = 7,
		SPEEDY       = 8
	};

	Bonus(const sf::Vector2f& pos, const unsigned short type);

	unsigned short getType() const { return type; }

	bool isExpired() const { return expireClock.getElapsedTime().asSeconds() >= EXPIRE_TIME; }

	void draw(sf::RenderTarget& window) override;
};

extern std::discrete_distribution<unsigned short> bonusTypeDistribution;
}
