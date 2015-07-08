#pragma once

#include <array>
#include <ostream>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include "Game.hpp"

template <class T, size_t ROWS, size_t COLS>
using Matrix = std::array<std::array<T, COLS>, ROWS>;

short keyToNumber(sf::Keyboard::Key key);

namespace Game {

inline float abs(float num) {
#ifdef IS_APPLE
// Apple Clang is a "good" compiler...
	if (num < 0) return -num;
	return num;
#else
	return std::abs(num);
#endif
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
		(float)(((unsigned short)pos.x/TILE_SIZE) * TILE_SIZE),
		(float)(((unsigned short)pos.y/TILE_SIZE) * TILE_SIZE));
}

}
