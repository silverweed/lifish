#include "Sighted.hpp"
#include "LevelManager.hpp"
#include "utils.hpp"
#include "Collider.hpp"

using Game::Sighted;

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

Sighted::Sighted(Game::Entity& owner, short visionRadius)
	: Game::Component(owner)
	, visionRadius(visionRadius)
{}

void Sighted::update() {
	Game::Component::update();
	if (lm == nullptr) return;

	for (unsigned short i = 0; i < (unsigned short)Game::Direction::NONE; ++i) {
		_fillLine(static_cast<Game::Direction>(i));	
	}
}

void Sighted::_fillLine(const Game::Direction dir) {
	// no check for lm != nullptr as it's done beforehand by update()

	const auto mtile = Game::tile(owner.getPosition());
	auto same_line = dir == Game::Direction::UP ? check_up :
			dir == Game::Direction::DOWN ? check_down :
			dir == Game::Direction::LEFT ? check_left : check_right;

	seen[dir].clear();

	lm->getEntities().apply([=] (const Game::Entity *e) {
		if (e == &owner) return;
		const auto etile = Game::tile(e->getPosition());
		if (!same_line(etile, mtile)) return;
		const auto dist = Game::manhattanDistance(etile, mtile);
		if (visionRadius < 0 || dist <= visionRadius) 
			seen[dir].push_back(std::make_pair(e, dist));
	});

	std::sort(seen[dir].begin(), seen[dir].end(), [] (
				const std::pair<const Game::Entity*, unsigned short> a,
				const std::pair<const Game::Entity*, unsigned short> b)
	{
		return a.second < b.second;
	});
	if (opaqueMask != 0) {
		// Discard entities farther than an opaque one
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

void Sighted::setOpaque(std::initializer_list<Game::Layers::Layer> layers, bool opaque) {
	for (auto layer : layers) {
		const auto shift = static_cast<unsigned>(layer);
		if (opaque)
			opaqueMask |= 1 << shift;
		else
			opaqueMask &= ~(1 << shift); 
	}
}

bool Sighted::_isOpaque(Game::Layers::Layer layer) const {
	return (opaqueMask >> static_cast<unsigned>(layer)) & 1;
}
