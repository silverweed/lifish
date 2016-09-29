#include "BossExplosion.hpp"
#include "Drawable.hpp"
#include "Animated.hpp"
#include "Temporary.hpp"
#include "utils.hpp"

using Game::BossExplosion;
using Game::TILE_SIZE;

BossExplosion::BossExplosion(const sf::Vector2f& pos) 
	: Game::Entity(pos)
{
	animated = addComponent(new Game::Animated(*this, Game::getAsset("test", "bossbullet.png")));
	addComponent(new Game::Drawable(*this, *animated));

	animated->addAnimation("explosion", {
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(4 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(5 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(6 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)
	}, true);
	
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.play();

	addComponent(new Game::Temporary(*this, [&animatedSprite] () {
		return !animatedSprite.isPlaying();
	}));
}
