#include "AxisSighted.hpp"
#include "utils.hpp"
#include "Collider.hpp"
#include "EntityGroup.hpp"

using Game::AxisSighted;

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


AxisSighted::AxisSighted(Game::Entity& owner, float visionRadius)
	: Game::Sighted(owner, visionRadius)
{}

void AxisSighted::update() {
	Game::Component::update();
	if (entities == nullptr) return;

	for (unsigned short i = 0; i < (unsigned short)Game::Direction::NONE; ++i) {
		_fillLine(static_cast<Game::Direction>(i));	
	}
}

void AxisSighted::_fillLine(const Game::Direction dir) {
	// no check for lm != nullptr as it's done beforehand by update()

	const auto mtile = Game::tile(owner.getPosition());
	auto same_line = dir == Game::Direction::UP ? check_up :
			dir == Game::Direction::DOWN ? check_down :
			dir == Game::Direction::LEFT ? check_left : check_right;

	seen[dir].clear();

	entities->apply([=] (const Game::Entity *e) {
		if (e == &owner) return;
		const auto etile = Game::tile(e->getPosition());
		if (!same_line(etile, mtile)) return;
		const auto dist = Game::manhattanDistance(etile, mtile);
		if (visionRadius < 0 || dist <= visionRadius) {
			// Only see living entities
			const auto killable = e->get<Game::Killable>();
			if (killable == nullptr || !killable->isKilled())
				seen[dir].push_back(std::make_pair(e, dist));
		}
	});

	std::sort(seen[dir].begin(), seen[dir].end(), [] (
				const std::pair<const Game::Entity*, unsigned short> a,
				const std::pair<const Game::Entity*, unsigned short> b)
	{
		return a.second < b.second;
	});
	if (opaqueMask != 0) {
		// Discard entities farther than an opaque one.
		// NOTE THAT at the moment only the first collider of the entity is used
		// to determine opaqueness; this assumes that we only see entities whose
		// first collider determines their bounding box.
		for (auto it = seen[dir].begin(); it != seen[dir].end(); ++it) {
			const auto cld = it->first->get<Game::Collider>();
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
