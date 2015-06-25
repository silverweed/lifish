#include "FixedWall.hpp"

using Game::FixedWall;

FixedWall::FixedWall(sf::Vector2f pos, const std::string& texture_name) 
	: Wall(pos, texture_name)
{
	// Fixed walls only have 1 sprite
	sprite.setTexture(texture);
}
