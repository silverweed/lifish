#include "Tile.hpp"

using namespace Game;

Tile::Tile(const Tile::Type type) : type(type) {}

Tile::Type Tile::getType() const {
	return type;
}
