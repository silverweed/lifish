#include "collision_utils.hpp"
#include "CompoundCollider.hpp"
#include "AxisMoving.hpp"

// Checks collision between a CompoundCollider and a Collider
bool Game::CollisionUtils::check_compound(const Game::CompoundCollider& cc1, 
		const Game::Collider& cld2, const Game::Direction dir) 
{
	for (const auto& c : cc1.getColliders())
		if (collide(cld2, c, dir))
			return true;
	return false;
}

// Checks if `cld1` and `cld2` collide, given that the owner of `cld1` has direction `dir`.
bool Game::CollisionUtils::collide(const Game::Collider& cld1, const Game::Collider& cld2, const Game::Direction dir) {
	// Special treatment is required for CompoundColliders: in this case, check singularly each sub-collider.
	if (auto cc1 = dynamic_cast<const Game::CompoundCollider*>(&cld1))
		return check_compound(*cc1, cld2, dir);
	else if (auto cc2 = dynamic_cast<const Game::CompoundCollider*>(&cld2))
		return check_compound(*cc2, cld1, dir); // XXX: should be oppositeDirection?

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
bool Game::CollisionUtils::is_at_boundaries(const Game::Collider& cld, const Game::AxisMoving *const am) {
	const auto pos = cld.getPosition();
	const auto rect = cld.getRect();
	if (am != nullptr) {
		auto dir = am->getDirection();
		switch (dir) {
		case Direction::UP:
			return pos.y <= TILE_SIZE;
		case Direction::LEFT:
			return pos.x <= TILE_SIZE;
		case Direction::DOWN:
			return pos.y + rect.height >= TILE_SIZE * (Game::LEVEL_HEIGHT + 1);
		case Direction::RIGHT:
			return pos.x + rect.width >= TILE_SIZE * (Game::LEVEL_WIDTH + 1);
		default:
			return false;
		}
	} else {
		return pos.y <= TILE_SIZE || pos.x <= TILE_SIZE 
			|| pos.x + rect.width >= TILE_SIZE * (Game::LEVEL_WIDTH + 1)
			|| pos.y + rect.height >= TILE_SIZE * (Game::LEVEL_HEIGHT + 1);
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
