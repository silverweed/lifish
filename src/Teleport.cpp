#include "Teleport.hpp"
#include "Game.hpp"

using Game::Teleport;
using Game::TILE_SIZE;

Teleport::Teleport(const sf::Vector2f& _pos) 
	//: FixedEntity(_pos, Game::getAsset("graphics", "teleport.png"))
	: FixedEntity(_pos, Game::getAsset("test", "teleport.png"))
{
	animations[0].setSpriteSheet(texture);

	// Teleports have 8 frames
	for (unsigned short i = 0; i < 8; ++i)
		animations[0].addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.08));
	animatedSprite.play();
}

void Teleport::draw(sf::RenderTarget& window) {
	if (disableCount == 0)
		animatedSprite.update(frameClock.restart());
	window.draw(animatedSprite);
}

void Teleport::disable() {
	disableCount = COOLDOWN_FRAMES;
}

int Teleport::waitCooldown() {
	if (disableCount == 0) return -1;
	return --disableCount;
}
