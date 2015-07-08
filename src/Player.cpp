#include "Player.hpp"
#include "utils.hpp"
#include <sstream>

using Game::Player;

Player::Player(sf::Vector2f pos, const unsigned short id) :
	// TODO: player sprites
	//MovingEntity(pos, Game::getAsset("graphics", std::string("player") + std::to_string(id) + std::string(".png"))), 
	MovingEntity(pos, Game::getAsset("test", std::string("player") + std::to_string(id) + std::string(".png"))), 
	Lifed(MAX_LIFE)
{
	speed = 150.f;
	transparentTo.players = false;

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

void Player::move() {
	move(direction);
}

void Player::move(const Direction dir) {
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
		switch (direction) {
		case Direction::RIGHT:
		case Direction::DOWN:
			if (Game::tile(pos) != prevAlign)
				pos = Game::aligned(pos);
			break;
		case Direction::LEFT:
			if (Game::tile(pos).x == prevAlign.x - 2)
				pos = Game::aligned(pos) + sf::Vector2f(Game::TILE_SIZE, 0);
			break;
		case Direction::UP:
			if (Game::tile(pos).y == prevAlign.y - 2)
				pos = Game::aligned(pos) + sf::Vector2f(0, Game::TILE_SIZE);
			break;
		case Direction::NONE:
			break;
		}
	} else {
		realign();
	}
	animatedSprite.update(frameTime);
}

bool Player::_isTransparentTo(const Entity *const e) const {
	return e->transparentTo.players;
}
