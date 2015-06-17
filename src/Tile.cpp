#include "Tile.hpp"

using Game::Tile;

Tile::Tile(const Tile::Type type) : type(type) {}

Tile::Type Tile::getType() const {
	return type;
}
