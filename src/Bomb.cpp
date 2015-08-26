#include "Bomb.hpp"
#include "Game.hpp"
#include "Player.hpp"

using Game::Bomb;
using Game::TILE_SIZE;

Bomb::Bomb(const sf::Vector2f& pos, const Game::Player *const source, 
		const unsigned short _fuseTime, const unsigned short _radius) :
	Game::FixedEntity(pos, Game::getAsset("test", "bomb.png")), 
	Game::Sounded({ Game::getAsset("sounds", "fuse.ogg") }),
	fuseTime(_fuseTime), 
	radius(_radius),
	sourcePlayer(source)
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
	if (fuseTime - fuseClock.getElapsedTime().asMilliseconds() < 2000 && !isExploded()) {
		Animation *anim = &animations[1];
		animatedSprite.play(*anim);
	}
	animatedSprite.update(frameClock.restart());
	window.draw(animatedSprite);
}

void Bomb::setExploding() {
	fuseTime = 50; 
	fuseClock.restart();
	ignited = true; 
}
