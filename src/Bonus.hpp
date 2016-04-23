#pragma once

#include <random>
#include <SFML/System.hpp>
#include "Sprite.hpp"
#include "Clock.hpp"
#include "Entity.hpp"

namespace Game {

extern std::discrete_distribution<unsigned short> bonusTypeDistribution;

/**
 * The bonuses dropped by walls
 */
class Bonus : public Game::Entity {
public:
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

private:
	const static sf::Time EXPIRE_TIME;
	constexpr static int VALUE = 100; // FIXME

	const Type type;

	Game::Clock<1> *expireClock = nullptr;
	Game::Sprite *sprite = nullptr;

public:
	constexpr static unsigned short N_BONUS_TYPES = 9;
	constexpr static unsigned short N_PERMANENT_BONUS_TYPES = 5;
	const static sf::Time SHIELD_DURATION;
	const static sf::Time SPEEDY_DURATION;

	Bonus(const sf::Vector2f& pos, const Type type);

	Type getType() const { return type; }

	bool isExpired() const { return expireClock->getElapsedTime() >= EXPIRE_TIME; }

	void update();
};

}
