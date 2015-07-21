#pragma once
/**
 * A boss
 */

#include "FixedEntity.hpp"
#include "Lifed.hpp"
#include "Scored.hpp"

namespace Game {

class Boss : public Game::FixedEntity, public Game::Lifed, public Game::Scored {
	constexpr static unsigned int VALUE = 5000;
	constexpr static unsigned short MAX_LIFE = 20; 
	constexpr static unsigned short N_SHOOTING_POINTS = 2;
	constexpr static unsigned short DEATH_TIME = 4000; // ms

	std::array<sf::Vector2f, N_SHOOTING_POINTS> shootingPoints;

	bool isHurt = false;
	sf::Clock hurtClock;

	bool dead = false;
public:
	/** A Boss is 3x3 tiles */
	constexpr static unsigned short SIZE = 3;
	constexpr static unsigned short SHOOT_INTERVAL = 2000; // ms
	constexpr static unsigned short SHOOT_SHORT_INTERVAL = 400; // ms
	/** Maximum range of each shooting point */
	constexpr static double MAX_RANGE = Game::TILE_SIZE * 9;

	Boss(const sf::Vector2f& pos);

	template<typename T> bool intersects(const sf::Rect<T>& rect) const {
		return sf::FloatRect(pos.x, pos.y, SIZE*TILE_SIZE, SIZE*TILE_SIZE).intersects(rect);
	}

	bool occupies(const sf::Vector2i& tile) const;

	const std::array<sf::Vector2f, N_SHOOTING_POINTS>& getShootingPoints() const {
		return shootingPoints;
	}

	std::array<double, N_SHOOTING_POINTS> getShootingAngles(const sf::Vector2f& pos) const;

	void hurt();

	void draw(sf::RenderTarget& window) override;

	/** Initiates kill animation */
	void kill();
	bool isDying() const { return dead; }
	bool isDead() const { return dead && hurtClock.getElapsedTime().asMilliseconds() > DEATH_TIME; }
};

}
