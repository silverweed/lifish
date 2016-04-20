#include "TransparentWall.hpp"
#include "Game.hpp"

using Game::TransparentWall;

TransparentWall::TransparentWall(const sf::Vector2f& pos)
	: BreakableWall(pos, Game::getAsset("graphics", "transparent_wall.png"), 1, SCORE) 
{
	//transparentTo.bullets = true;
}
