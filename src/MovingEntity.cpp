#include "MovingEntity.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include <iostream>

using Game::MovingEntity;
using Game::TILE_SIZE;
using Game::Direction;

MovingEntity::MovingEntity(sf::Vector2f pos, const std::string& texture_name) 
	: Entity(pos, texture_name)
{
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

void MovingEntity::move(const Direction dir) {
	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock.restart();

	Animation *anim;

	switch (dir) {
	case Direction::UP:
		anim = &animations[ANIM_UP];
		shift.y -= speed;
		break;
	case Direction::LEFT:
		anim = &animations[ANIM_LEFT];
		shift.x -= speed;
		break;
	case Direction::DOWN:
		anim = &animations[ANIM_DOWN];
		shift.y += speed;
		break;
	case Direction::RIGHT:
		anim = &animations[ANIM_RIGHT];
		shift.x += speed;
		break;
	}

        animatedSprite.play(*anim);
        animatedSprite.move(shift * frameTime.asSeconds());
	animatedSprite.update(frameTime);
}

void MovingEntity::stop() {
	animatedSprite.setAnimation(animations[ANIM_DOWN]);
	animatedSprite.stop();
	animatedSprite.update(frameClock.restart());
}
