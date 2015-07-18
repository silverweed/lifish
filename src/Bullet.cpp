#include "Bullet.hpp"
#include "Game.hpp"
#include "utils.hpp"

using Game::Bullet;

Bullet::Bullet(const sf::Vector2f& _pos, const Game::Direction dir, unsigned short id,
		float _speed, unsigned short _damage, short _range) :
	Game::MovingEntity(_pos, Game::getAsset("test", "bullets.png")),
	range(_range),
	origin(_pos),
	damage(_damage),
	shift(0.f, 0.f)
{
	direction = dir;
	speed = BASE_SPEED * _speed;
	canTeleport = false;

	// Bullets have a variable number of frames, up to 13:
	// motion frames: 1 ~ 8 (max 8 / directional per direction)
	// destroy frames: 0 ~ 5
	animations[0].setSpriteSheet(texture);
	animations[ANIM_DEATH].setSpriteSheet(texture);
	switch (id) {
	case 1:
		// shot
		directionality = 1;
		size = 7;
		break;
	case 2:
		// fireball
		directionality = 1;
		size = 13;
		break;
	case 3:
		// MG shot
		directionality = 4;
		nMotionFrames = 1;
		nDestroyFrames = 5;
		size = 10;
		break;
	case 4:
		// lightbolt
		directionality = 1;
		size = 13;
		break;
	case 5:
		// flame
		directionality = 2;
		nMotionFrames = 5;
		nDestroyFrames = 0;
		size = TILE_SIZE;
		break;
	case 6:
		// plasma
		directionality = 1;
		size = 12;
		break;
	case 7:
		// magma
		directionality = 4;
		size = 26;
		break;
	default:
		break;
	}

	unsigned short d = 0;
	switch (dir) {
	case Game::Direction::DOWN: 
		d = 0;
		pos.x += (TILE_SIZE - size) / 2;
		pos.y += TILE_SIZE;
		break;
	case Game::Direction::UP:    
		d = directionality == 4 ? 1 : 0;
		pos.x += (TILE_SIZE - size) / 2;
		break;
	case Game::Direction::RIGHT:
		d = directionality == 4 ? 2 : directionality == 2 ? 1 : 0; 
		pos.y += (TILE_SIZE - size) / 2;
		pos.x += TILE_SIZE;
		break;
	case Game::Direction::LEFT: 
		d = directionality == 4 ? 3 : directionality == 2 ? 1 : 0;
		pos.y += (TILE_SIZE - size) / 2;
		break;
	default: 
		break;
	}

	// We use animations[0] as motion animation and animations[ANIM_DEATH] for destruction animation.
	// Since the bullet cannot change direction, we only need to setup these 2 animations.
	// The motion animation has up to 8 / directionality frames (but can also have less),
	// while the destroy animation can have from 0 to 5 frames.
	// Format for the spritesheet is:
	// 	- if directionality == 1, [motion frames] [destroy frames]
	//	- if == 2, [up/down frames] [left/right frames] [destroy frames]
	//	- if == 4, [down] [up] [right] [left] [destroy]
	unsigned short j = 0;
	for (unsigned short i = 0; i < nMotionFrames && i < 8 / directionality; ++i)
		animations[0].addFrame(sf::IntRect((nMotionFrames * d + j++) * TILE_SIZE, (id-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	// destroy animations are non-directional
	for (unsigned short i = j; i < j + nDestroyFrames && i < j + 5; ++i)
		animations[ANIM_DEATH].addFrame(sf::IntRect(i * TILE_SIZE, (id-1) * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[0]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.play();

	switch (direction) {
	case Direction::UP:
		shift.y -= speed;
		break;
	case Direction::LEFT:
		shift.x -= speed;
		break;
	case Direction::DOWN:
		shift.y += speed;
		break;
	case Direction::RIGHT:
		shift.x += speed;
		break;
	case Direction::NONE:
		return;
	}

}

bool Bullet::_isTransparentTo(const Entity *const e) const {
	return e->transparentTo.bullets;
}

void Bullet::move() {
	if (direction == Game::Direction::NONE) {
		animatedSprite.update(frameClock.restart());
		return;
	}
	if (Game::abs(pos.x - origin.x) > Game::TILE_SIZE * range || Game::abs(pos.y - origin.y) > Game::TILE_SIZE * range) {
		destroy();
		return;
	}

	sf::Time frameTime = frameClock.restart();

	if (!colliding) {
		animatedSprite.move(shift * frameTime.asSeconds());
		pos = animatedSprite.getPosition();
		// Ensure we are always aligned at least for one frame for
		// each tile we step in (this may not be the case if FPS are too low)
		_ensureAlign();
	} 
	animatedSprite.update(frameTime);
}

void Bullet::move(const Direction) {
	move();
}

void Bullet::draw(sf::RenderTarget& window) {
	Game::Animated::draw(window);
}

bool Bullet::hits(const sf::Vector2f& epos) const {
	sf::FloatRect me(pos.x, pos.y, size, size);
	sf::FloatRect it(epos.x, epos.y, TILE_SIZE, TILE_SIZE);
	return me.intersects(it);
}

void Bullet::destroy() {
	if (!destroyed) {
		if (nDestroyFrames > 0) {
			switch (direction) {
			case Game::Direction::UP: case Game::Direction::DOWN:
				pos.x = Game::aligned(pos).x;
				break; 
			default:
				pos.y = Game::aligned(pos).y;
				break; 
			}
			animatedSprite.setPosition(pos);
			animatedSprite.play(*&animations[ANIM_DEATH]);
		}
		animatedSprite.setLooped(false);
		destroyed = true;
		direction = Game::Direction::NONE;
	}
}
