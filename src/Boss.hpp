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

	std::array<sf::Vector2f, N_SHOOTING_POINTS> shootingPoints;
public:
	/** A Boss is 3x3 tiles */
	constexpr static unsigned short SIZE = 3;
	constexpr static unsigned short SHOOT_INTERVAL = 3000; // ms
	/** Maximum range of each shooting point */
	constexpr static double MAX_RANGE = Game::TILE_SIZE * 9;

	Boss(const sf::Vector2f& pos);

	template<typename T> bool intersects(const sf::Rect<T>& rect) {
		return sf::FloatRect(pos.x, pos.y, SIZE*TILE_SIZE, SIZE*TILE_SIZE).intersects(rect);
	}

	const std::array<sf::Vector2f, N_SHOOTING_POINTS>& getShootingPoints() const {
		return shootingPoints;
	}

	std::array<double, N_SHOOTING_POINTS>&& getShootingAngles(const sf::Vector2f& pos) const;
};

}
