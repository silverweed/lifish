#include "Flash.hpp"
#include "Game.hpp"

using Game::Flash;

Flash::Flash(const sf::Vector2f& pos) 
	: Game::Temporary(pos, Game::getAsset("test", "flash.png"))
{
	animations[0].setSpriteSheet(*texture);
	animations[0].addFrame(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.pause();
}
