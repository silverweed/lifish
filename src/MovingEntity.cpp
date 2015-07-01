#include "MovingEntity.hpp"
#include "Game.hpp"
#include "GameCache.hpp"

using Game::MovingEntity;
using Game::TILE_SIZE;

MovingEntity::MovingEntity(sf::Vector2f pos, const std::string& texture_name) 
	: Entity(pos)
{
	Game::cache.loadTexture(texture, texture_name);

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

	animatedSprite.setAnimation(animations[ANIM_DOWN]);
	animatedSprite.setLooped(true);
	animatedSprite.pause();
}

void MovingEntity::draw(sf::RenderTarget& window) {
	// TODO
	animatedSprite.setPosition(pos);
	window.draw(animatedSprite);
}
