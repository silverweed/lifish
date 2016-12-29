#include "TransparentWall.hpp"
#include "game.hpp"
#include "Collider.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"

using Game::TransparentWall;

TransparentWall::TransparentWall(const sf::Vector2f& pos)
	: BreakableWall(pos, Game::getAsset("graphics", "transparent_wall.png"), 1, SCORE) 
{
	get<Game::Collider>()->setLayer(Game::Layers::TRANSP_WALLS);
	get<Game::ZIndexed>()->setZIndex(Game::Conf::ZIndex::TRANSP_WALLS);
}
