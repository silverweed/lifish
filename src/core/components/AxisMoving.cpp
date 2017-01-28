#include "AxisMoving.hpp"
#include "core.hpp"
#include "utils.hpp"
#include "Clock.hpp"
#include "Shooting.hpp"
#include <exception>

using lif::AxisMoving;
using lif::TILE_SIZE;

AxisMoving::AxisMoving(lif::Entity& owner, float speed, lif::Direction dir)
	: lif::Moving(owner, speed)
	, direction(dir)
	, prevDirection(lif::Direction::NONE)
	, prevAlign(-1.f, -1.f)
{
	moving = dir != lif::Direction::NONE;
}

void AxisMoving::update() {
	lif::Component::update();
	if (!moving || _handleBlock()) return;

	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock->restart();

	// Cap frameTime to a maximum to avoid excessive "jumps" due to lag.
	if (frameTime > MAX_FRAME_TIME)
		frameTime = MAX_FRAME_TIME;
	
	const float effSpeed = _effectiveSpeed();

	switch (direction) {
	case Direction::UP:
		shift.y -= effSpeed;
		break;
	case Direction::LEFT:
		shift.x -= effSpeed;
		break;
	case Direction::DOWN:
		shift.y += effSpeed;
		break;
	case Direction::RIGHT:
		shift.x += effSpeed;
		break;
	case Direction::NONE:
		return;
	}

	const float delta = effSpeed * frameTime.asSeconds();
	distTravelled += delta;

	if (!_collidesWithSolid()) {
		owner.setPosition(owner.getPosition() + shift * frameTime.asSeconds());
		if (delta > 1 && ensureAlign)
			_ensureAlign(); 
	} else if (autoRealign) {
		realign();
	}

	prevDirection = direction;
}

// Realigns the entity by "bouncing it back" to the tile it occupies the most.
// If direction is NONE, just aligns it to its current tile
void AxisMoving::realign() {
	auto pos = owner.getPosition();

	switch (direction) {
	case lif::Direction::UP:
		pos = sf::Vector2f(pos.x, (static_cast<int>(((pos.y-1) / TILE_SIZE) + 1)) * TILE_SIZE);
		break;
	case lif::Direction::LEFT:
		pos = sf::Vector2f((static_cast<int>(((pos.x-1) / TILE_SIZE) + 1)) * TILE_SIZE, pos.y);
		break;
	case lif::Direction::DOWN:
		pos = sf::Vector2f(pos.x, static_cast<int>((pos.y / TILE_SIZE)) * TILE_SIZE);
		break;
	case lif::Direction::RIGHT:
		pos = sf::Vector2f(static_cast<int>((pos.x / TILE_SIZE)) * TILE_SIZE, pos.y);
		break;
	default: 
		pos = lif::aligned(pos);
		break;
	}
	
	owner.setPosition(pos);
}

void AxisMoving::stop() {
	lif::Moving::stop();
	direction = prevDirection = lif::Direction::NONE;
	if (autoRealign)
		realign();
}

void AxisMoving::setDirection(lif::Direction dir) {
	if (dir == direction) return;
	const auto pos = owner.getPosition();
	switch (dir) {
		case Direction::UP:
		case Direction::DOWN:
			owner.setPosition(sf::Vector2f(static_cast<int>(pos.x), pos.y));
			break;
		case Direction::LEFT: 
		case Direction::RIGHT:
			owner.setPosition(sf::Vector2f(pos.x, static_cast<int>(pos.y)));
			break;
		default:
			break;
	}

	direction = dir; 
	distTravelled = 0;
	moving = dir != lif::Direction::NONE;
}

void AxisMoving::turn(short straightAngles, bool clockwise) {
	if (!clockwise)
		straightAngles *= -1;

	direction = lif::turnRight(direction, straightAngles);
}

void AxisMoving::_ensureAlign() {
	// Ensure we are always aligned at least for one frame for
	// each tile we step in (this may not be the case if FPS are too low)
	auto pos = owner.getPosition();
	switch (direction) {
	case Direction::RIGHT:
	case Direction::DOWN:
		if (lif::tile(pos) != prevAlign)
			pos = lif::aligned(pos);
		break;
	case Direction::LEFT:
		if (lif::tile(pos).x == prevAlign.x - 2)
			pos = lif::aligned(pos) + sf::Vector2f(lif::TILE_SIZE, 0);
		break;
	case Direction::UP:
		if (lif::tile(pos).y == prevAlign.y - 2)
			pos = lif::aligned(pos) + sf::Vector2f(0, lif::TILE_SIZE);
		break;
	case Direction::NONE:
		return;
	}
	owner.setPosition(pos);
}
