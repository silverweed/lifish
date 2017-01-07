#include "AxisSighted.hpp"
#include "utils.hpp"
#include "Collider.hpp"
#include "EntityGroup.hpp"

using lif::AxisSighted;

// Helper functions for _fillLine
static bool check_up(const sf::Vector2i& etile, const sf::Vector2i& mtile) {
	return etile.x == mtile.x && etile.y < mtile.y;
}

static bool check_down(const sf::Vector2i& etile, const sf::Vector2i& mtile) {
	return etile.x == mtile.x && etile.y > mtile.y;
}

static bool check_left(const sf::Vector2i& etile, const sf::Vector2i& mtile) {
	return etile.y == mtile.y && etile.x < mtile.x;
}

static bool check_right(const sf::Vector2i& etile, const sf::Vector2i& mtile) {
	return etile.y == mtile.y && etile.x > mtile.x;
}
// end helper functions


AxisSighted::AxisSighted(lif::Entity& owner, float visionRadius)
	: lif::Sighted(owner, visionRadius)
{}

void AxisSighted::update() {
	lif::Component::update();
	if (entities == nullptr) return;

	for (unsigned short i = 0; i < (unsigned short)lif::Direction::NONE; ++i) {
		_fillLine(static_cast<lif::Direction>(i));	
	}
}

void AxisSighted::_fillLine(const lif::Direction dir) {
	// no check for lm != nullptr as it's done beforehand by update()

	const auto mtile = lif::tile2(owner.getPosition());
	auto same_line = dir == lif::Direction::UP ? check_up :
			dir == lif::Direction::DOWN ? check_down :
			dir == lif::Direction::LEFT ? check_left : check_right;

	seen[dir].clear();

	entities->apply([=] (std::weak_ptr<lif::Entity> e) {
		if (e.expired())
			return;
		auto ptr = e.lock();
		if (ptr.get() == &owner)
			return;
		const auto etile = lif::tile2(ptr->getPosition());
		if (!same_line(etile, mtile)) return;
		const auto dist = lif::manhattanDistance(etile, mtile);
		if (visionRadius < 0 || dist <= visionRadius) {
			// Only see living entities
			const auto killable = ptr->get<lif::Killable>();
			if (killable == nullptr || !killable->isKilled())
				seen[dir].push_back(std::make_pair(ptr, dist));
		}
	});

	std::sort(seen[dir].begin(), seen[dir].end(), [] (
				const std::pair<std::weak_ptr<lif::Entity>, unsigned short> a,
				const std::pair<std::weak_ptr<lif::Entity>, unsigned short> b)
	{
		return a.second < b.second;
	});
	if (opaqueMask != 0) {
		// Discard entities farther than an opaque one.
		// NOTE THAT at the moment only the first collider of the entity is used
		// to determine opaqueness; this assumes that we only see entities whose
		// first collider determines their bounding box.
		for (auto it = seen[dir].begin(); it != seen[dir].end(); ++it) {
			const auto cld = it->first.lock()->get<lif::Collider>();
			if (cld != nullptr) {
				const auto layer = cld->getLayer();
				if (_isOpaque(layer)) {
					seen[dir].erase(it, seen[dir].end());
					break;
				}
			}
		}
	}
}
