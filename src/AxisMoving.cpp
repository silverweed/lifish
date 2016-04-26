#include "AxisMoving.hpp"
#include "Game.hpp"
#include "utils.hpp"

using Game::AxisMoving;
using Game::TILE_SIZE;

AxisMoving::AxisMoving(Game::Entity *const owner, float speed, Game::Direction dir)
	: Game::Moving(owner, speed)
	, direction(dir)
	, prevDirection(Game::Direction::NONE)
{}

void AxisMoving::update() {
	if (!moving) return;

	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock->restart();

	if (frameTime > MAX_FRAME_TIME)
		frameTime = MAX_FRAME_TIME;
	
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

	//if (!colliding) {
		owner->setPosition(owner->getPosition() + shift * frameTime.asSeconds());
		distTravelled += speed * frameTime.asSeconds();
	//}
}

void AxisMoving::_realign() {
	sf::Vector2f pos;

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

	owner->setPosition(pos);
}

void AxisMoving::stop() {
	Game::Moving::stop();
	direction = prevDirection = Game::Direction::NONE;
	_realign();
}
