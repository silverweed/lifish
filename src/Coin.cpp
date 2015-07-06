#include <cmath>
#include "Coin.hpp"
#include "Game.hpp"

using Game::Coin;
using Game::TILE_SIZE;

Coin::Coin(const sf::Vector2f& pos, const std::string& texture_name)
	: FixedEntity(pos, texture_name)
{
	texture.setSmooth(true);
}
