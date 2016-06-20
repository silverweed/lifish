#pragma once

#include <array>
#include "Clock.hpp"
#include "Sprite.hpp"

namespace Game {

/**
 * A boss is a fixed entity occupying `SIZE` x `SIZE` tiles in the level.
 * It is endued with `N_SHOOTING_POINTS` coordinates whence it fires
 * its BossBullets.
 */
class Boss : public Game::Entity {
	constexpr static unsigned int VALUE = 5000;
	constexpr static unsigned short MAX_LIFE = 20; 
	constexpr static unsigned short N_SHOOTING_POINTS = 2;
	constexpr static unsigned short DEATH_TIME = 4000; // ms

	std::array<sf::Vector2f, N_SHOOTING_POINTS> shootingPoints;
	Game::Clock *hurtClock = nullptr,
		    *explClock = nullptr;
	Game::Sprite *sprite = nullptr;

	bool isHurt = false;

public:
	/** A Boss is 3x3 tiles */
	constexpr static unsigned short SIZE = 3;
	constexpr static unsigned short SHOOT_INTERVAL = 2000; // ms
	constexpr static unsigned short SHOOT_SHORT_INTERVAL = 400; // ms
	/** Maximum range of each shooting point */
	constexpr static double MAX_RANGE = Game::TILE_SIZE * 9;

	explicit Boss(const sf::Vector2f& pos);

	const std::array<sf::Vector2f, N_SHOOTING_POINTS>& getShootingPoints() const {
		return shootingPoints;
	}

	void update() override;

	void hurt();

	/** Initiates kill animation */
	void kill();
	bool isDying() const { return dead; }
	bool isDead() const { return dead && hurtClock.getElapsedTime().asMilliseconds() > DEATH_TIME; }
};

}
