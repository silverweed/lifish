#include "TransparentWall.hpp"
#include "Game.hpp"
#include "Collider.hpp"

using Game::TransparentWall;

TransparentWall::TransparentWall(const sf::Vector2f& pos)
	: BreakableWall(pos, Game::getAsset("graphics", "transparent_wall.png"), 1, SCORE) 
{
	get<Game::Collider>()->setLayer(Game::Layers::TRANSP_WALLS);
}
