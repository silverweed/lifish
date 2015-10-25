#include "BossBullet.hpp"
#include "Game.hpp"

using Game::BossBullet;
using Game::TILE_SIZE;

BossBullet::BossBullet(const sf::Vector2f& pos, double _angle)
	: Bullet(pos, Game::getAsset("test", "bossbullet.png"))
	, angle(_angle)
{
	directionality = 1;
	nMotionFrames = 2;
	nDestroyFrames = 5;
	transparentToWalls = true;
	speed = 160.f;
	size = 20;
	damage = 4;

	// Set a random direction different from NONE to make move() work
	direction = Game::Direction::UP;

	shift.x = speed * std::cos(angle);
	shift.y = speed * std::sin(angle);

	animations[0].addFrame(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	animations[0].addFrame(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DEATH].addFrame(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DEATH].addFrame(sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DEATH].addFrame(sf::IntRect(4 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DEATH].addFrame(sf::IntRect(5 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DEATH].addFrame(sf::IntRect(6 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setFrameTime(sf::seconds(0.06));
	animatedSprite.setLooped(true);
	animatedSprite.play();
}
