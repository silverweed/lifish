#include <cmath>
#include "Coin.hpp"
#include "Game.hpp"

using Game::Coin;
using Game::TILE_SIZE;

Coin::Coin(const sf::Vector2f& pos)
	: Game::Animated(pos, Game::getAsset("test", "coin.png"))
	, Game::Scored(VALUE)
	, Game::Sounded({ Game::getAsset("sounds", "coin.ogg") })
{
	texture.setSmooth(true);

	animations[0].setSpriteSheet(texture);
	// Coins have 10 sprites
	for (unsigned short i = 0; i < 10; ++i)
		animations[0].addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.02));
	animatedSprite.pause();

	_addClock(&grabClock);
}

void Coin::grab() {
	if (!grabbed) {
		grabbed = true;
		grabClock.restart();
		animatedSprite.play();
	}
}

void Coin::draw(sf::RenderTarget& window) {
	if (grabbed) {
		animatedSprite.update(frameClock.restart());
	}
	window.draw(animatedSprite);
}

bool Coin::isGrabbed() const {
	return grabbed && grabClock.getElapsedTime().asMilliseconds() > GRAB_TIME; 
}
