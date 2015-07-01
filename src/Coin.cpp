#include <cmath>
#include "Coin.hpp"
#include "Game.hpp"

using Game::Coin;
using Game::TILE_SIZE;

Coin::Coin(sf::Vector2f pos, const std::string& texture_name)
	: FixedEntity(pos, texture_name)
{
	texture.setSmooth(true);
	//sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
	//sprite.setPosition(pos + sf::Vector2f(TILE_SIZE/2, TILE_SIZE/2));
}
