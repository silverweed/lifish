#include "collision_utils.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"

using Game::TILE_SIZE;

// Checks if `cld1` and `cld2` collide, given that the owner of `cld1` has direction `dir`.
bool Game::CollisionUtils::collide(const Game::Collider& cld1, const Game::Collider& cld2, const Game::Direction dir) {
	sf::IntRect rect = cld1.getRect(),
		    orect = cld2.getRect();

	switch (dir) {
	case Direction::UP:
		--rect.top;
		break;
	case Direction::DOWN:
		++rect.top;
		break;
	case Direction::LEFT:
		--rect.left;
		break;
	case Direction::RIGHT:
		++rect.left;
		break;
	default:
		break;
	}

	return rect.intersects(orect);
}

// Checks if `cld` is at the level limit. Algorithm used depends on whether
// that Entity is AxisMoving or not.
bool Game::CollisionUtils::is_at_boundaries(const Game::Collider& cld,
		const Game::AxisMoving *const am, const sf::FloatRect& limits)
{
	const auto pos = cld.getPosition();
	const auto rect = cld.getRect();
	if (am != nullptr) {
		auto dir = am->getDirection();
		switch (dir) {
		case Direction::UP:
			return pos.y <= limits.top;
		case Direction::LEFT:
			return pos.x <= limits.left;
		case Direction::DOWN:
			return pos.y + rect.height >= limits.height;
		case Direction::RIGHT:
			return pos.x + rect.width >= limits.width;
		default:
			return false;
		}
	} else {
		return pos.y <= limits.top || pos.x <= limits.left
			|| pos.x + rect.width >= limits.width
			|| pos.y + rect.height >= limits.height;
	}
}

// Checks if `ocld` is along the forward direction of `cld`
bool Game::CollisionUtils::direction_is_viable(const Game::Collider& cld, 
		const Game::AxisMoving& moving, const Game::Collider& ocld)
{
	const auto pos = cld.getPosition();
	const auto opos = ocld.getPosition();
	const auto size = cld.getRect();
	const auto osize = ocld.getRect();

	switch (moving.getDirection()) {
	case Direction::UP: return opos.y + osize.height <= pos.y + TILE_SIZE;
	case Direction::DOWN: return opos.y >= pos.y + size.height - TILE_SIZE;
	case Direction::LEFT: return opos.x + osize.width <= pos.x + TILE_SIZE;
	case Direction::RIGHT: return opos.x >= pos.x + size.width - TILE_SIZE;
	default: break;
	}

	return true;
}
