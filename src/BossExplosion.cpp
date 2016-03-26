#include "BossExplosion.hpp"

using Game::BossExplosion;

BossExplosion::BossExplosion(const sf::Vector2f& pos) :
	Game::Temporary(pos, Game::getAsset("test", "bossbullet.png")),
	Game::Sounded({ Game::getAsset("test", "bossbullet_hit.ogg") })
{
	animations[0].setSpriteSheet(*texture);
	animations[0].addFrame(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(4 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(5 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(6 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.pause();
}
