#include "FixedWall.hpp"
#include "game.hpp"
#include "Sprite.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "Fixed.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"

using Game::FixedWall;
using Game::TILE_SIZE;

FixedWall::FixedWall(const sf::Vector2f& pos, const unsigned short id) 
	: Game::Entity(pos)
{
	addComponent(new Game::Fixed(*this));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::WALLS));
	addComponent(new Game::Drawable(*this,
			*addComponent(new Game::Sprite(*this, Game::getAsset("graphics", "fixed.png"),
				sf::IntRect((id-1) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)))));
	addComponent(new Game::Collider(*this, Game::Layers::UNBREAKABLES));
}
