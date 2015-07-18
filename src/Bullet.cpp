#include "Bullet.hpp"

using Game::Bullet;

Bullet::Bullet(const sf::Vector2f& pos, unsigned short id, float _speed, unsigned short _damage, short _range) :
	Game::MovingEntity(pos, Game::getAsset("test", "bullets.png")),
	range(_range),
	damage(_damage)
{
	speed = BASE_SPEED * _speed;
	canTeleport = false;
	transparentTo.enemies = false;
	transparentTo.players = false;

	// Bullets have a variable number of frames, up to 13:
	// motion frames: 1 ~ 8 (max 8 / directional per direction)
	// destroy frames: 0 ~ 5
	animations[0].setSpriteSheet(texture);
	switch (id) {
	case 1:
		// shot
		directionality = 1;
		break;
	default:
		break;
	}

	unsigned short j = 0;
	switch (directionality) {
	case 4:
		for (unsigned short dir = 0; dir < 4; ++dir) {
			// up, right, down, left
			for (unsigned short i = 0; i < nMotionFrames && i < 2; ++i)
				animations[dir].addFrame(sf::IntRect((j++) * TILE_SIZE, (id-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE));
		}
		break;
	case 2:
		for (unsigned short dir = 0; dir < 2; ++dir) {
			// up/down, left/right
			for (unsigned short i = 0; i < nMotionFrames && i < 4; ++i) {
				animations[dir].addFrame(sf::IntRect((j++) * TILE_SIZE, (id-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE));
				animations[(dir+2)%4].addFrame(sf::IntRect((j++) * TILE_SIZE, (id-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE));
			}
		}
		break;
	case 1:
		// same animation for all directions
		for (unsigned short dir = 0; dir < 4; ++dir) {
			for (unsigned short i = 0; i < nMotionFrames && i < 8; ++i)
				animations[dir].addFrame(sf::IntRect((j++) * TILE_SIZE, (id-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE));
		}
		break;
	default:
		break;
	}

	// destroy animations are non-directional
	for (unsigned short i = j; i < j + nDestroyFrames; ++i)
		animations[ANIM_DEATH].addFrame(sf::IntRect(i * TILE_SIZE, (id-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setLooped(false);
	animatedSprite.pause();
}

bool Bullet::_isTransparentTo(const Entity *const e) const {
	return e->transparentTo.bullets;
}
