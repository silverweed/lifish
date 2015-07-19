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
	constexpr static unsigned short MAX_LIFE = 20; // FIXME
public:
	/** A Boss is 3x3 tiles */
	constexpr static unsigned short SIZE = 3;

	Boss(const sf::Vector2f& pos);

	template<typename T> bool intersects(const sf::Rect<T>& rect) {
		return sf::FloatRect(pos.x, pos.y, SIZE*TILE_SIZE, SIZE*TILE_SIZE).intersects(rect);
	}
};

}
