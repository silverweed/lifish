#include "AlienSprite.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "utils.hpp"

using Game::AlienSprite;
using Game::Direction;

AlienSprite::AlienSprite()
	: Game::Animated(sf::Vector2f(0, 0))
	, Game::Sounded({ Game::getAsset("test", "alien_death.ogg") })
{
	texture = Game::cache.loadTexture(Game::getAsset("test", "aliensprite.png"));
}
