#include "BreakableWall.hpp"
#include "Game.hpp"
#include "GameCache.hpp"

using Game::BreakableWall;
using Game::TILE_SIZE;

BreakableWall::BreakableWall(sf::Vector2f pos, const unsigned short id, unsigned short _life) 
	: Wall(pos), Lifed(_life)
{
	Game::cache.loadTexture(texture, Game::getAsset("graphics", "breakable.png"));
	// Breakable walls have 4 sprites: the "animatedSprites" array is used
	// to keep these sprites, and the animation starts when the wall is being destroyed.
	animations[0].setSpriteSheet(texture);
	animations[0].addFrame(sf::IntRect(0, (id-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(TILE_SIZE, (id-1), TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(2 * TILE_SIZE, (id-1), TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(3 * TILE_SIZE, (id-1), TILE_SIZE, TILE_SIZE));

	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setLooped(false);
	animatedSprite.pause();
}

void BreakableWall::draw(sf::RenderTarget& window) {
	animatedSprite.setPosition(pos);
	window.draw(animatedSprite);
}
