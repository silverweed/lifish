#include "Flash.hpp"
#include "game.hpp"
#include "Drawable.hpp"
#include "Temporary.hpp"

using Game::Flash;
using Game::TILE_SIZE;

Flash::Flash(const sf::Vector2f& pos) 
	: Game::Entity(pos)
{
	animated = addComponent(new Game::Animated(*this, Game::getAsset("test", "flash.png")));
	addComponent(new Game::Drawable(*this, animated));

	animated->addAnimation("flash", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)
	}, true);

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.play();

	addComponent(new Game::Temporary(*this, [&animatedSprite] () {
		return !animatedSprite.isPlaying();
	}));
}
