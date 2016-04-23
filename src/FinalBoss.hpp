#pragma once

#include <array>
#include "LifedMovingEntity.hpp"
#include "Scored.hpp"
#include "Clocked.hpp"

namespace Game {

/**
 * The Final Boss is like a Boss which can move and spawn enemies.
 * For design issues, this cannot be made a child class of Boss.
 */
class FinalBoss : public Game::LifedMovingEntity, public Game::Scored {
	constexpr static unsigned int VALUE = 100000;
	constexpr static unsigned short MAX_LIFE = 100; // FIXME
	constexpr static unsigned short DEATH_TIME = 6000; // ms
	constexpr static unsigned short BREED_TIME = 5000; // ms
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static unsigned short MIN_DIST_BEFORE_CHANGE_DIR = Game::TILE_SIZE * 3;

	bool isHurt = false;
	sftools::Chronometer hurtClock;

	sftools::Chronometer breedClock;

	bool dead = false;

	bool _isTransparentTo(const Entity *const e) const override {
		return e->transparentTo.enemies;
	}

public:
	constexpr static unsigned short SHOOT_INTERVAL = 5000; // ms
	constexpr static unsigned short SIZE = 5;
	
	/** The clock used to spawn BossExplosions upon this boss's death */
	sftools::Chronometer explClock;

	FinalBoss(const sf::Vector2f& pos);

	template<typename T> bool intersects(const sf::Rect<T>& rect) const {
		return sf::FloatRect(pos.x, pos.y, SIZE*TILE_SIZE, SIZE*TILE_SIZE).intersects(rect);
	}

	bool occupies(const sf::Vector2i& tile) const;

	void hurt();

	void draw(sf::RenderTarget& window) override;

	bool canGo(const Game::Direction dir, const Game::LevelManager *const lr) const override;

	/** Initiates kill animation */
	void kill() override;
	bool isDying() const { return dead; }
	bool isDead() const { return dead && hurtClock.getElapsedTime().asMilliseconds() > DEATH_TIME; }

	bool shouldBreed() const { return breedClock.getElapsedTime().asMilliseconds() > BREED_TIME; }
	void breed();

	void chooseDirection(const Game::LevelManager *const lr);
	void detectCollisions(const Game::LevelManager *const lr);
};

}
