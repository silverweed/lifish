#pragma once

#include <array>
#include <SFML/Window/Keyboard.hpp>
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
