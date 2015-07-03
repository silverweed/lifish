#include "MovingEntity.hpp"
#include "Game.hpp"
#include "GameCache.hpp"

using Game::MovingEntity;
using Game::TILE_SIZE;

MovingEntity::MovingEntity(sf::Vector2f pos, const std::string& texture_name) 
	: Entity(pos, texture_name)
{
	transparentTo.bullets = false;

	for (unsigned short i = 0; i < MAX_N_ANIMATIONS; ++i)
		animations[i].setSpriteSheet(texture);

	for (unsigned short i = 0; i < WALK_N_FRAMES; ++i) {
		animations[ANIM_DOWN].addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
		animations[ANIM_UP].addFrame(sf::IntRect(i * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
		animations[ANIM_RIGHT].addFrame(sf::IntRect(i * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
		animations[ANIM_LEFT].addFrame(sf::IntRect(i * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
		if (i > DEATH_N_FRAMES + 1) continue;
		unsigned short k = ANIM_DEATH;
		if (i == DEATH_N_FRAMES) k = ANIM_WIN;
		else if (i == DEATH_N_FRAMES + 1) k = ANIM_HURT;
		animations[k].addFrame(sf::IntRect(i * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	}

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[ANIM_DOWN]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.06));
	animatedSprite.pause();
}

void MovingEntity::move() {
	move(direction);
}

void MovingEntity::move(const Direction dir) {
	bool must_align = false;
	if (moving && prevDirection != dir) {
		// Entities are allowed to change direction only when
		// they're aligned to a grid cell.
		unsigned short ix = (unsigned short)pos.x,
			       iy = (unsigned short)pos.y;
		if (ix % TILE_SIZE != 0 || iy % TILE_SIZE != 0) {
			must_align = true;
		} else {
			// we're already (almost) aligned: ensure we're exactly
			// at integral coordinates.
			pos.x = ix, pos.y = iy;
			animatedSprite.setPosition(pos);
		}
	}
	
	moving = true;

	if (must_align) {
		// We're not aligned yet with the grid: go on
		// in the current direction.
		direction = prevDirection;
	} else {
		// Can change direction safely
		prevDirection = direction = dir;
	}

	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock.restart();

	Animation *anim;
		 
	switch (direction) {
	case DIR_UP:
		anim = &animations[ANIM_UP];
		if (!colliding[DIR_UP])
			shift.y -= speed;
		break;
	case DIR_LEFT:
		anim = &animations[ANIM_LEFT];
		if (!colliding[DIR_LEFT])
			shift.x -= speed;
		break;
	case DIR_DOWN:
		anim = &animations[ANIM_DOWN];
		if (!colliding[DIR_DOWN])
			shift.y += speed;
		break;
	case DIR_RIGHT:
		anim = &animations[ANIM_RIGHT];
		if (!colliding[DIR_RIGHT])
			shift.x += speed;
		break;
	case DIR_NONE:
		return;
	}
	
        animatedSprite.play(*anim);
        animatedSprite.move(shift * frameTime.asSeconds());
	pos = animatedSprite.getPosition();
	animatedSprite.update(frameTime);
}

void MovingEntity::stop() {
	animatedSprite.setAnimation(animations[ANIM_DOWN]);
	animatedSprite.stop();
	animatedSprite.update(frameClock.restart());
	moving = false;
}
