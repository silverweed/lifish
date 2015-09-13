#include "TransparentWall.hpp"

using Game::TransparentWall;

TransparentWall::TransparentWall(const sf::Vector2f& pos)
	: BreakableWall(pos, Game::getAsset("graphics", "transparent_wall.png")) 
{
	transparentTo.bullets = true;
	pointsGiven = 20;
}
