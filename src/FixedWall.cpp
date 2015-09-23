#include "FixedWall.hpp"
#include "Game.hpp"

using Game::FixedWall;

FixedWall::FixedWall(sf::Vector2f pos, const unsigned short id) 
	: Game::NonAnimated(pos, Game::getAsset("graphics", "fixed.png"))
{
	transparentTo.players = false;
	transparentTo.enemies = false;
	transparentTo.bullets = false;
	transparentTo.explosions = false;

	// Fixed walls only have 1 sprite
	sprite.setTextureRect(sf::IntRect((id-1) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
}
