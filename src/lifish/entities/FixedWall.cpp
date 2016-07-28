#include "FixedWall.hpp"
#include "game.hpp"
#include "Sprite.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "Fixed.hpp"

using Game::FixedWall;
using Game::TILE_SIZE;

FixedWall::FixedWall(const sf::Vector2f& pos, const unsigned short id) 
	: Game::Entity(pos)
{
	addComponent(new Game::Fixed(*this));
	addComponent(new Game::Drawable(*this,
			*addComponent(new Game::Sprite(*this, Game::getAsset("graphics", "fixed.png"),
				sf::IntRect((id-1) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)))));
	addComponent(new Game::Collider(*this, Game::Layers::WALLS));

	// TODO collision layers
	//transparentTo.players = false;
	//transparentTo.enemies = false;
	//transparentTo.bullets = false;
	//transparentTo.explosions = false;
}
