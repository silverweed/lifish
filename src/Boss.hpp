#pragma once

#include <array>
#include "NonAnimated.hpp"
#include "Lifed.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "Clocked.hpp"

namespace Game {

/**
 * A boss is a fixed entity occupying `SIZE` x `SIZE` tiles in the level.
 * It is endued with `N_SHOOTING_POINTS` coordinates whence it fires
 * its BossBullets.
 */
class Boss
	: public Game::NonAnimated
	, public Game::Lifed
	, public Game::Scored
	, public Game::Sounded
	, public Game::Clocked
{
	constexpr static unsigned int VALUE = 5000;
	constexpr static unsigned short MAX_LIFE = 20; 
	constexpr static unsigned short N_SHOOTING_POINTS = 2;
	constexpr static unsigned short DEATH_TIME = 4000; // ms

	std::array<sf::Vector2f, N_SHOOTING_POINTS> shootingPoints;

	bool isHurt = false;
	sftools::Chronometer hurtClock;

	bool dead = false;

public:
	/** A Boss is 3x3 tiles */
	constexpr static unsigned short SIZE = 3;
	constexpr static unsigned short SHOOT_INTERVAL = 2000; // ms
	constexpr static unsigned short SHOOT_SHORT_INTERVAL = 400; // ms
	/** Maximum range of each shooting point */
	constexpr static double MAX_RANGE = Game::TILE_SIZE * 9;

	/** The clock used to spawn BossExplosions upon this boss's death */
	sftools::Chronometer explClock;

	Boss(const sf::Vector2f& pos);

	template<typename T> bool intersects(const sf::Rect<T>& rect) const {
		return sf::FloatRect(pos.x, pos.y, SIZE*TILE_SIZE, SIZE*TILE_SIZE).intersects(rect);
	}

	bool occupies(const sf::Vector2i& tile) const;

	const std::array<sf::Vector2f, N_SHOOTING_POINTS>& getShootingPoints() const {
		return shootingPoints;
	}

	void hurt();

	void draw(sf::RenderTarget& window) override;

	/** Initiates kill animation */
	void kill();
	bool isDying() const { return dead; }
	bool isDead() const { return dead && hurtClock.getElapsedTime().asMilliseconds() > DEATH_TIME; }
};

}
