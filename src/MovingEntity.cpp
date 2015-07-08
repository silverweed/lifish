#include "MovingEntity.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "LevelRenderer.hpp"

using Game::MovingEntity;
using Game::TILE_SIZE;
using Game::Direction;

std::ostream& Game::operator<<(std::ostream& stream, const Direction& dir) {
	switch (dir) {
	case Direction::UP: stream << "UP"; break;
	case Direction::LEFT: stream << "LEFT"; break;
	case Direction::DOWN: stream << "DOWN"; break;
	case Direction::RIGHT: stream << "RIGHT"; break;
	default: stream << "NONE"; break;
	}
	return stream;
}

MovingEntity::MovingEntity(const sf::Vector2f& pos, const std::string& texture_name) 
	: Entity(pos, texture_name)
{
	transparentTo.bullets = false;
}

void MovingEntity::move() {
	move(direction);
}

void MovingEntity::move(const Direction dir) {
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
		// Ensure we are always aligned at least for one frame for
		// each tile we step in (this may not be the case if FPS are too low)
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
	} 
	animatedSprite.update(frameTime);
}

void MovingEntity::stop() {
	animatedSprite.setAnimation(animations[ANIM_DOWN]);
	animatedSprite.stop();
	animatedSprite.update(frameClock.restart());
	moving = false;
	direction = prevDirection = Game::Direction::NONE;
	realign();
}

void MovingEntity::realign() {
	switch (direction) {
	case Game::Direction::UP:
		pos = sf::Vector2f(pos.x, ((unsigned short)((pos.y-1) / TILE_SIZE) + 1) * TILE_SIZE);
		break;
	case Game::Direction::LEFT:
		pos = sf::Vector2f(((unsigned short)((pos.x-1) / TILE_SIZE) + 1) * TILE_SIZE, pos.y);
		break;
	case Game::Direction::DOWN:
		pos = sf::Vector2f(pos.x, (unsigned short)(pos.y / TILE_SIZE) * TILE_SIZE);

		break;
	case Game::Direction::RIGHT:
		pos = sf::Vector2f((unsigned short)(pos.x / TILE_SIZE) * TILE_SIZE, pos.y);
		break;
	default: 
		pos = Game::aligned(pos);
		break;
	}
	animatedSprite.setPosition(pos);
}

bool MovingEntity::canGo(const Direction dir, const Game::LevelRenderer *const lr) const {
	short iposx = (short)(pos.x / TILE_SIZE) - 1,
	      iposy = (short)(pos.y / TILE_SIZE) - 1;
	
	switch (dir) {
	case Direction::UP:
		--iposy;
		break;
	case Direction::LEFT:
		--iposx;
		break;
	case Direction::DOWN:
		++iposy;
		break;
	case Direction::RIGHT:
		++iposx;
		break;
	default: return true;
	}
	if (iposx < 0 || iposx >= LEVEL_WIDTH || iposy < 0 || iposy >= LEVEL_HEIGHT)
		return false;

	short idx = iposy * LEVEL_WIDTH + iposx;
	auto fixed = lr->getFixedEntities();
	
	return (fixed[idx] == nullptr || _isTransparentTo(fixed[idx]));
}

void MovingEntity::setDirection(const Direction dir) {
	if (dir == direction) return;
	switch (dir) {
	case Direction::UP: case Direction::DOWN:
		pos.x = (unsigned short)pos.x;
		break;
	case Direction::LEFT: case Direction::RIGHT:
		pos.y = (unsigned short)pos.y;
		break;
	case Direction::NONE: break;
	}
	animatedSprite.setPosition(pos);
	direction = dir;
}
