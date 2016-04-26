#include <cmath>
#include "Coin.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "Game.hpp"

using Game::Coin;
using Game::TILE_SIZE;

const sf::Time Coin::GRAB_TIME = sf::milliseconds(3000);

Coin::Coin(const sf::Vector2f& pos)
	: Game::Entity(pos)
{
	addComponent(new Game::Scored(this, VALUE));
	addComponent(new Game::Sounded(this, { Game::getAsset("sounds", "coin.ogg") }));
	grabClock = addComponent(new Game::Clock<1>(this));
	animated = addComponent(new Game::Animated(this, Game::getAsset("graphics", "coin.png")));
	animated->get<Game::Texture>()->getTexture()->setSmooth(true);

	auto& anim = animated->addAnimation("spin");

	// Coins have 10 sprites
	for (unsigned short i = 0; i < 10; ++i)
		anim.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(anim);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.02));
	animatedSprite.pause();
}

void Coin::grab() {
	if (!grabbed) {
		grabbed = true;
		grabClock->restart();
		animated->getSprite().play();
	}
}

void Coin::update() {
	if (grabbed)
		animated->update();
}

bool Coin::isGrabbed() const {
	return grabbed && grabClock->getElapsedTime() > GRAB_TIME; 
}
