#include "Teleport.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "Drawable.hpp"

using Game::Teleport;
using Game::TILE_SIZE;

const sf::Time Teleport::COOLDOWN_TIME = sf::milliseconds(1000);

Teleport::Teleport(const sf::Vector2f& pos) 
	: Game::Entity(pos)
{
	animated = addComponent(new Game::Animated(this, Game::getAsset("graphics", "teleport.png")));
	disableClock = addComponent(new Game::Clock<1>(this));
	addComponent(new Game::Drawable(this, animated));

	auto& anim = animated->addAnimation("teleport");
	// Teleports have 8 frames
	for (unsigned short i = 0; i < 8; ++i)
		anim.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(anim);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.15));
	animatedSprite.play();
}

void Teleport::update() {
	if (disabled && disableClock->getElapsedTime() >= COOLDOWN_TIME)
		disabled = false;

	if (!disabled)
		animated->update();
}

void Teleport::disable() {
	disabled = true;
	disableClock->restart();
}
