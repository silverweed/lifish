#include "Sighted.hpp"
#include "LevelManager.hpp"
#include "utils.hpp"

using Game::Sighted;

// Helper functions for _fillLine
static void _checkRight(const Game::Entity *e, const sf::Vector2i& mtile, 
		const Game::Entity *nearest, unsigned short *dist)
{
	const auto etile = Game::tile(e->getPosition());
	if (etile.y != mtile.y) return;
	if (nearest == nullptr) {
		nearest = e;
		*dist = etile.x - mtile.x;
		return;
	}
	if (e->getPosition().x < nearest->getPosition().x)
		nearest = e;
}

static void _checkLeft(const Game::Entity *e, const sf::Vector2i& mtile,
		const Game::Entity *nearest, unsigned short *dist)
{
	const auto etile = Game::tile(e->getPosition());
	if (etile.y != mtile.y) return;
	if (nearest == nullptr) {
		nearest = e;
		*dist = mtile.x - etile.x;
		return;
	}
	if (e->getPosition().x > nearest->getPosition().x)
		nearest = e;
}

static void _checkUp(const Game::Entity *e, const sf::Vector2i& mtile,
		const Game::Entity *nearest, unsigned short *dist)
{
	const auto etile = Game::tile(e->getPosition());
	if (etile.x != mtile.x) return;
	if (nearest == nullptr) {
		nearest = e;
		*dist = mtile.y - etile.y;
		return;
	}
	if (e->getPosition().y > nearest->getPosition().y)
		nearest = e;
}

static void _checkDown(const Game::Entity *e, const sf::Vector2i& mtile,
		const Game::Entity *nearest, unsigned short *dist)
{
	const auto etile = Game::tile(e->getPosition());
	if (etile.x != mtile.x) return;
	if (nearest == nullptr) {
		nearest = e;
		*dist = etile.y - mtile.y;
		return;
	}
	if (e->getPosition().y < nearest->getPosition().y)
		nearest = e;
}
// end helper functions

Sighted::Sighted(Game::Entity *const owner, short visionRadius)
	: Game::Component(owner)
	, visionRadius(visionRadius)
{
	seen.fill(std::make_pair(nullptr, 0));
}

void Sighted::update() {
	if (lm == nullptr || !owner->isAligned()) return;

	for (unsigned short i = 0; i < (unsigned short)Game::Direction::NONE; ++i) {
		seen[i] = _fillLine(lm, static_cast<Game::Direction>(i));	
	}
}

auto Sighted::_fillLine(const Game::LevelManager *lm, const Game::Direction dir) 
		-> std::pair<const Game::Entity*, unsigned short> 
{
	auto mtile = Game::tile(owner->getPosition());
	short r = 1; // distance of currently looked tile
	auto func = dir == Game::Direction::UP ? _checkUp :
		    dir == Game::Direction::DOWN ? _checkDown :
		    dir == Game::Direction::RIGHT ? _checkRight : _checkLeft;
	Game::Entity *nearest = nullptr;
	unsigned short distance = Game::LEVEL_WIDTH + 1;

	while (visionRadius < 0 || r < visionRadius) {
		lm->getEntities().apply(func, mtile, nearest, &distance);
	}

	return std::make_pair(nearest, distance);
}
