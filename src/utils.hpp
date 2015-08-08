#pragma once

#include <array>
#include <ostream>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include "Game.hpp"
#include "MovingEntity.hpp"
#include "Animated.hpp"

short keyToNumber(sf::Keyboard::Key key);

namespace Game {

template <class T, size_t ROWS, size_t COLS>
using Matrix = std::array<std::array<T, COLS>, ROWS>;

template<typename T> inline T abs(T num) {
#ifdef IS_APPLE
	// Apple Clang is a "good" compiler...
	if (num < 0) return -num;
	return num;
#else
	return std::abs(num);
#endif
}

inline unsigned short directionToUshort(const Direction dir) {
	switch (dir) {
	case Game::Direction::UP:    return ANIM_UP;
	case Game::Direction::DOWN:  return ANIM_DOWN;
	case Game::Direction::LEFT:  return ANIM_LEFT;
	case Game::Direction::RIGHT: return ANIM_RIGHT;
	default:                     return ANIM_DEATH;
	}
}

/** Returns the aligned tile corresponding to position `pos` by
 *  flooring its components and dividing by TILE_SIZE.
 */
inline sf::Vector2i tile(const sf::Vector2f& pos) {
	return sf::Vector2i(
			(unsigned short)pos.x / Game::TILE_SIZE, 
			(unsigned short)pos.y / Game::TILE_SIZE);
}

inline sf::Vector2f aligned(const sf::Vector2f& pos) {
	return sf::Vector2f(
		float(((unsigned short)pos.x/TILE_SIZE) * TILE_SIZE),
		float(((unsigned short)pos.y/TILE_SIZE) * TILE_SIZE));
}

template<typename T>
inline std::ostream& operator<<(std::ostream& stream, const sf::Vector2<T>& vec) {
	return stream << "(" << vec.x << ", " << vec.y << ")";
}

template<typename T>
inline std::ostream& operator<<(std::ostream& stream, const sf::Rect<T>& rect) {
	return stream << "(" << rect.left << ", " << rect.top 
		<< " x " << rect.width << ", " << rect.height << ")";
}

inline double distance(const sf::Vector2f& a, const sf::Vector2f& b) {
	return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

} // end namespace Game
