#include "Position.hpp"

using Game::Position;

bool Position::isAligned(const char axis) {
	switch (axis) {
		case 'x': return (unsigned short)pos.x % Game::TILE_SIZE == 0;
		case 'y': return (unsigned short)pos.y % Game::TILE_SIZE == 0;
	}
	return (unsigned short)pos.x % Game::TILE_SIZE == 0 
		&& (unsigned short)pos.y % Game::TILE_SIZE == 0;
}
