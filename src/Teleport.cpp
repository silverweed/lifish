#include "Teleport.hpp"
#include "Game.hpp"
#include "GameCache.hpp"

using Game::Teleport;
using Game::TILE_SIZE;

Teleport::Teleport(const sf::Vector2f& _pos) 
	: Game::Animated(_pos, Game::getAsset("graphics", "teleport.png"))
{
	animations[0].setSpriteSheet(*texture);

	// Teleports have 8 frames
	for (unsigned short i = 0; i < 8; ++i)
		animations[0].addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.15));
	animatedSprite.play();

	_addClock(&disableClock);
}

void Teleport::draw(sf::RenderTarget& window) {
	sf::Time frameTime = frameClock.restart();
	if (!isDisabled())
		animatedSprite.update(frameTime);
	window.draw(animatedSprite);
}

void Teleport::disable() {
	disabled = true;
	disableClock.restart();
}

bool Teleport::isDisabled() {
	if (!disabled) return false;
	if ((unsigned int)disableClock.getElapsedTime().asMilliseconds() >= COOLDOWN_TIME) {
		disabled = false;
		return false;
	}
	return true;
}
