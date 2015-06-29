#include <cmath>
#include "Coin.hpp"
#include "Game.hpp"

using Game::Coin;
using Game::TILE_SIZE;

Coin::Coin(sf::Vector2f pos, const std::string& texture_name)
	: FixedEntity(pos, texture_name)
{
	// Coins have 6 sprites
	sprite.setTexture(texture);
	texture.setSmooth(true);
	sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
	sprite.setPosition(pos + sf::Vector2f(TILE_SIZE/2, TILE_SIZE/2));
}
void Coin::draw(sf::RenderTarget& window) {
	//sprite.setScale(std::abs(cos(Game::frameClock.getElapsedTime().asSeconds()*4)), 1);
	window.draw(sprite);
}
