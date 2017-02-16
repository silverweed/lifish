#include "FixedWall.hpp"
#include "game.hpp"
#include "Sprite.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "Fixed.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"

using lif::FixedWall;
using lif::TILE_SIZE;

FixedWall::FixedWall(const sf::Vector2f& pos, const unsigned short id) 
	: lif::Entity(pos)
{
	addComponent(std::make_shared<lif::Fixed>(*this));
	addComponent(std::make_shared<lif::ZIndexed>(*this, lif::conf::zindex::WALLS));
	addComponent(std::make_shared<lif::Drawable>(*this,
			*addComponent(std::make_shared<lif::Sprite>(*this, lif::getAsset("graphics", "fixed.png"),
				sf::IntRect((id-1) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)))));
	addComponent(std::make_shared<lif::Collider>(*this, lif::c_layers::UNBREAKABLES));
}
