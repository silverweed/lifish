#include "Enemy.hpp"

using Game::Enemy;

Enemy::Enemy(sf::Vector2f pos, const unsigned short id) :
	// TODO: enemy sprites
	//MovingEntity(pos, Game::getAsset("graphics", std::string("player") + std::to_string(id) + std::string(".png"))), 
	MovingEntity(pos, Game::getAsset("test", std::string("enemy") + std::to_string(id) + std::string(".png"))), 
	Lifed(1)
{
	speed = 75.f;
	transparentTo.enemies = false;
	transparentTo.bullets = false;
	direction = prevDirection = Game::Direction::DOWN;

	for (unsigned short i = 0; i < MAX_N_ANIMATIONS; ++i) {
		animations[i].setSpriteSheet(texture);
		if (i < 4)
			shootFrame[i].setTexture(texture);
	}

	for (unsigned short i = 0; i < WALK_N_FRAMES; ++i) {
		animations[ANIM_DOWN].addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
		animations[ANIM_UP].addFrame(sf::IntRect((WALK_N_FRAMES + i) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
		animations[ANIM_RIGHT].addFrame(sf::IntRect(i * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
		animations[ANIM_LEFT].addFrame(sf::IntRect((WALK_N_FRAMES + i) * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	}
	shootFrame[ANIM_DOWN].setTextureRect(sf::IntRect(0, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	shootFrame[ANIM_UP].setTextureRect(sf::IntRect(TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	shootFrame[ANIM_RIGHT].setTextureRect(sf::IntRect(2 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	shootFrame[ANIM_LEFT].setTextureRect(sf::IntRect(3 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DEATH].addFrame(sf::IntRect(WALK_N_FRAMES * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animations[ANIM_DEATH].addFrame(sf::IntRect((WALK_N_FRAMES + 1) * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[ANIM_DOWN]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.12));
	animatedSprite.pause();
}
void Enemy::move(const Direction dir) {
	moving = true;
	direction = dir;

	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock.restart();

	Animation *anim;
		 
	switch (direction) {
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
	case Direction::NONE:
		return;
	}

        animatedSprite.play(*anim);
	if (!colliding) {
		animatedSprite.move(shift * frameTime.asSeconds());
		pos = animatedSprite.getPosition();
	} else {
	}
	animatedSprite.update(frameTime);
}
