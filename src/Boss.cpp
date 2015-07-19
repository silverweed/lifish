#include "Boss.hpp"
#include "Game.hpp"

using Game::Boss;

Boss::Boss(const sf::Vector2f& pos) :
	Game::FixedEntity(pos, Game::getAsset("test", "boss.png")),
	Game::Lifed(MAX_LIFE),
	Game::Scored(VALUE)
{
	sprite.setTextureRect(sf::IntRect(0, 0, SIZE * Game::TILE_SIZE, SIZE * Game::TILE_SIZE));
}
