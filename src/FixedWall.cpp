#include "FixedWall.hpp"
#include "Game.hpp"
#include "GameCache.hpp"

using Game::FixedWall;

FixedWall::FixedWall(sf::Vector2f pos, const unsigned short id) 
	: Wall(pos, Game::getAsset("graphics", "fixed.png"))
{
	// Fixed walls only have 1 sprite
	sprite.setTextureRect(sf::IntRect((id-1) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
}
