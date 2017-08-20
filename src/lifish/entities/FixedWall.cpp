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
	addComponent<lif::Fixed>(*this);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::WALLS);
	addComponent<lif::Drawable>(*this,
			*addComponent<lif::Sprite>(*this, lif::getAsset("graphics", "fixed.png"),
				sf::IntRect((id-1) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)));
	addComponent<lif::Collider>(*this, lif::c_layers::UNBREAKABLES);
}
