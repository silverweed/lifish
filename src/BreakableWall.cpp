#include "BreakableWall.hpp"
#include "Game.hpp"
#include "GameCache.hpp"

using Game::BreakableWall;
using Game::TILE_SIZE;

BreakableWall::BreakableWall(const sf::Vector2f& pos, const unsigned short id, unsigned short _life) 
	: Wall(pos, Game::getAsset("graphics", "breakable.png")), Lifed(_life)
{
	// Breakable walls have 4 sprites: the "animatedSprites" array is used
	// to keep these sprites, and the animation starts when the wall is being destroyed.
	animations[0].setSpriteSheet(texture);
	for (unsigned short i = 0; i < 4; ++i)
		animations[0].addFrame(sf::IntRect(i * TILE_SIZE, (id-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setLooped(false);
	animatedSprite.pause();
}
