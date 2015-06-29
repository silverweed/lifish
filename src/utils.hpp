#pragma once

#include <array>
#include <SFML/Window/Keyboard.hpp>

template <class T, size_t ROWS, size_t COLS>
using Matrix = std::array<std::array<T, COLS>, ROWS>;

short keyToNumber(sf::Keyboard::Key key);
