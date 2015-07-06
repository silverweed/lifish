#pragma once

#include <array>
#include <ostream>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include "Game.hpp"

template <class T, size_t ROWS, size_t COLS>
using Matrix = std::array<std::array<T, COLS>, ROWS>;

short keyToNumber(sf::Keyboard::Key key);

// Apple Clang is a "good" compiler...
#ifdef IS_APPLE
namespace Game {

template<typename T>
T abs(const T& num) {
	if (num < 0) return -num;
	return num;
}

}
#else
#define HAVE_STD_ABS
#endif

/** Returns the aligned tile corresponding to position `pos` by
 *  flooring its components and dividing by TILE_SIZE.
 */
sf::Vector2i tile(const sf::Vector2f& pos);
