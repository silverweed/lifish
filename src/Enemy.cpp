#include "Enemy.hpp"

using Game::Enemy;

Enemy::Enemy(sf::Vector2f pos, const unsigned short id) :
	// TODO: enemy sprites
	//MovingEntity(pos, Game::getAsset("graphics", std::string("player") + std::to_string(id) + std::string(".png"))), 
	Game::LifedMovingEntity(pos, Game::getAsset("test", std::string("enemy") + std::to_string(id) + std::string(".png")), 1), 
	Game::Scored(id * 100)
{
	speed = BASE_SPEED;
	transparentTo.enemies = false;
	transparentTo.bullets = false;
	direction = prevDirection = Game::Direction::DOWN;

	unsigned short death_n_frames = 2;
	switch (id) {
	case 3:
		death_n_frames = 4;
		break;
	default:
		break;
	}

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

	for (unsigned short i = 0; i < death_n_frames; ++i) 
		animations[ANIM_DEATH].addFrame(sf::IntRect((WALK_N_FRAMES + i) * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	animatedSprite.setPosition(pos);
	animatedSprite.setAnimation(animations[ANIM_DOWN]);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.12));
	animatedSprite.pause();
}

bool Enemy::_isTransparentTo(const Entity *const e) const {
	return e->transparentTo.enemies;
}

bool Enemy::isRecharging() const {
	return attackClock.getElapsedTime().asSeconds() < 1./attack.fireRate;
}

void Enemy::shoot() {
	attackClock.restart();
	animatedSprite.setAnimation(animations[directionToUshort(direction)]);
	shooting = true;
}

void Enemy::draw(sf::RenderTarget& window) {
	if (shooting) {
		if (attackClock.getElapsedTime().asMilliseconds() < shootFrameTime) {
			const unsigned short d = Game::directionToUshort(direction);
			shootFrame[d].setPosition(pos);
			window.draw(shootFrame[d]);
			return;
		} else {
			shooting = false;
		}
	}
	Game::MovingEntity::draw(window);
}

void Enemy::move(const Direction dir) {
	if (blocked) {
		if (attackClock.getElapsedTime().asMilliseconds() < attack.blockTime) {
			animatedSprite.update(frameClock.restart());
			return;
		}
		blocked = false;
	}
	MovingEntity::move(dir);
}
