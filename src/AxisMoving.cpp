#include "AxisMoving.hpp"
#include "Game.hpp"
#include "utils.hpp"
#include "Shooting.hpp"
#include "LevelManager.hpp"
#include <exception>

using Game::AxisMoving;
using Game::TILE_SIZE;

AxisMoving::AxisMoving(Game::Entity *const owner, float speed, Game::Direction dir)
	: Game::Moving(owner, speed)
	, direction(dir)
	, prevDirection(Game::Direction::NONE)
	, prevAlign(-1.f, -1.f)
{
	moving = dir != Game::Direction::NONE;
}

void AxisMoving::update() {
	Game::Component::update();

	if (!moving) return;

	if (prevDirection != direction)
		realign();

	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock->restart();

	// Cap frameTime to a maximum to avoid excessive "jumps" due to lag.
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

	// TODO
	if (!_collidesWithSolid()) {
		owner->setPosition(owner->getPosition() + shift * frameTime.asSeconds());
		const float delta = speed * frameTime.asSeconds();
		distTravelled += delta;
		if (delta > 1)
			_ensureAlign();
	} else realign();

	prevDirection = direction;

	// FIXME: why cannot set this here but must do it after entities.updateAll()?
	if (owner->isAligned())
		prevAlign = Game::tile(owner->getPosition());
}

// Realigns the entity by "bouncing it back" to the tile it occupies the most.
// If direction is NONE, just aligns it to its current tile
void AxisMoving::realign() {
	auto pos = owner->getPosition();

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
	realign();
}

bool AxisMoving::canGo(const Game::Direction dir, const Game::LevelManager& lm) const {
	auto pos = owner->getPosition();
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
	default:
		return true;
	}

	if (iposx < 0 || iposx >= LEVEL_WIDTH || iposy < 0 || iposy >= LEVEL_HEIGHT)
		return false;

	const short idx = iposy * LEVEL_WIDTH + iposx;

	// TODO
	/*
	const auto& fixed = lm->getFixedEntities();
	
	if (fixed[idx] != nullptr && !_isTransparentTo(fixed[idx]))
		return false;

	const auto bosses = lm->getBosses();
	const sf::FloatRect r(iposx, iposy, TILE_SIZE, TILE_SIZE);
	for (auto& boss : bosses)
		if (boss->intersects(r)) return false;
	*/

	return true;
}

void AxisMoving::setDirection(Game::Direction dir) {
	if (dir == direction) return;
	direction = dir; 

	moving = dir != Game::Direction::NONE;
	realign();
	if (!moving) stop();
	//auto pos = owner->getPosition();
	//switch (dir) {
	//case Direction::UP: case Direction::DOWN:
		//pos.x = (unsigned short)pos.x;
		//break;
	//case Direction::LEFT: case Direction::RIGHT:
		//pos.y = (unsigned short)pos.y;
		//break;
	//case Direction::NONE: break;
	//}
	//owner->setPosition(pos);
}

void AxisMoving::turn(short straightAngles, bool clockwise) {
	if (!clockwise)
		straightAngles *= -1;

	direction = Game::turnRight(direction, straightAngles);
}

void AxisMoving::_ensureAlign() {
	// Ensure we are always aligned at least for one frame for
	// each tile we step in (this may not be the case if FPS are too low)
	auto pos = owner->getPosition();
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
		return;
	}
	owner->setPosition(pos);
}
