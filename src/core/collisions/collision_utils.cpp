#include "collision_utils.hpp"
#include "AxisMoving.hpp"
#include "BaseLevelManager.hpp"
#include "Collider.hpp"

using lif::TILE_SIZE;

// Checks if `cld1` and `cld2` collide, given that the owner of `cld1` has direction `dir`.
bool lif::collision_utils::collide(const lif::Collider& cld1, const lif::Collider& cld2, const lif::Direction dir) {
	auto rect = static_cast<sf::IntRect>(cld1.getRect()),
	     orect = static_cast<sf::IntRect>(cld2.getRect());

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
bool lif::collision_utils::isAtBoundaries(const lif::Collider& cld,
		const lif::AxisMoving *const am, const sf::FloatRect& limits)
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
bool lif::collision_utils::directionIsViable(const lif::Collider& cld,
		const lif::AxisMoving& moving, const lif::Collider& ocld)
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

// Finds all free tiles that don't collide with any non-phantom collider in lm
std::vector<sf::Vector2i> lif::collision_utils::findFreeTiles(const lif::BaseLevelManager& lm) {
	const auto limits = lm.getCollisionDetector().getLevelLimit();
	std::vector<sf::Vector2i> free;
	free.reserve(limits.width / lif::TILE_SIZE * limits.height / lif::TILE_SIZE);
	for (int x = limits.left; x < limits.width; x += lif::TILE_SIZE)
		for (int y = limits.top; y < limits.height; y += lif::TILE_SIZE)
			free.emplace_back(x / lif::TILE_SIZE, y / lif::TILE_SIZE);

	const auto touchedTiles = [] (const auto& cld) {
		const auto pos = cld->getOwner().getPosition();
		const auto size = cld->getSize();
		std::vector<sf::Vector2i> touched;
		touched.reserve(size.x / lif::TILE_SIZE * size.y / lif::TILE_SIZE);
		for (int x = 0; x < size.x; x += lif::TILE_SIZE)
			for (int y = 0; y < size.y; y += lif::TILE_SIZE)
				touched.emplace_back(lif::tile(pos + sf::Vector2f(x, y)));
		touched.emplace_back(lif::tile(pos + sf::Vector2f(size.x - 1, size.y - 1)));
		return touched;
	};
	for (auto wcld : lm.getEntities().getColliding()) {
		if (wcld.expired()) continue;
		const auto cld = wcld.lock();
		const auto touched = touchedTiles(cld);
		free.erase(std::remove_if(free.begin(), free.end(), [&touched] (const auto& pos) {
			return std::find(touched.begin(), touched.end(), pos) != touched.end();
		}), free.end());
	}
	return free;
}
