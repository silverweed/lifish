#include "Bomb.hpp"
#include "Game.hpp"

using Game::Bomb;
using Game::TILE_SIZE;

Bomb::Bomb(const sf::Vector2f& pos, const float _fuseTime, const unsigned short _radius)
	: FixedEntity(pos, Game::getAsset("test", "bomb.png")), fuseTime(_fuseTime), radius(_radius)
{
	animations[0].setSpriteSheet(texture);
	animations[1].setSpriteSheet(texture);
	animations[0].addFrame(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[1].addFrame(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	animations[1].addFrame(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setPosition(pos);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.05));
	animatedSprite.play();
}

void Bomb::draw(sf::RenderTarget& window) {
	if (ignited && fuseTime - fuseClock.getElapsedTime().asSeconds() < 2 && !exploded()) {
		Animation *anim = &animations[1];
		animatedSprite.play(*anim);
	}
	animatedSprite.update(frameClock.restart());
	window.draw(animatedSprite);
}
