#include "FixedWall.hpp"
#include "Game.hpp"
#include "Position.hpp"
#include "Sprite.hpp"

using Game::FixedWall;
using Game::TILE_SIZE;

FixedWall::FixedWall(const sf::Vector2f& pos, const unsigned short id) 
{
	addComponent(new Game::Position<float>(this, pos));
	addComponent(new Game::Sprite(this, Game::getAsset("graphics", "fixed.png"),
				sf::IntRect((id-1) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)));

	// TODO collision layers
	//transparentTo.players = false;
	//transparentTo.enemies = false;
	//transparentTo.bullets = false;
	//transparentTo.explosions = false;
}
