#include "BombFormationsManager.hpp"
#include "Direction.hpp"
#include "game.hpp"
#include <unordered_set>
#include <iostream>
#include "BaseLevelManager.hpp"
#include "Bomb.hpp"
#include "DebugPainter.hpp"
#include <SFML/Graphics.hpp>

namespace {

constexpr auto LEVEL_WIDTH = lif::GAME_WIDTH / lif::TILE_SIZE - 2;
constexpr auto LEVEL_HEIGHT = lif::GAME_HEIGHT / lif::TILE_SIZE - 2;

using Grid = char[LEVEL_HEIGHT * LEVEL_WIDTH];

struct Vector2iHash
{
    std::size_t operator()(const sf::Vector2i& s) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(s.x);
        std::size_t h2 = std::hash<int>{}(s.y);
        return h1 ^ (h2 << 1);
    }
};

void dumpBombsToGrid(const lif::BaseLevelManager& blm, Grid grid) {
	memset(grid, '.', LEVEL_HEIGHT * LEVEL_WIDTH);

	blm.getEntities().apply([grid] (const lif::Entity& e) {
		if (dynamic_cast<const lif::Bomb *>(&e)) {
			auto tile = lif::tile(e.getPosition());
			grid[(tile.y - 1) * LEVEL_WIDTH + (tile.x - 1)] = '*';
		}
	});
}

}

#include "bomb_patterns.inl"

void lif::BombFormation::addBomb(sf::Vector2i newTile) {
	for (const auto& tile : tiles) {
		signature += 1.f / lif::sqrDistance(tile, newTile);
	}
	tiles.push_back(newTile);
}

void lif::BombFormationsManager::addBomb(const lif::BaseLevelManager& blm, sf::Vector2i start) {
	Grid grid;
	dumpBombsToGrid(blm, grid);
	for (int row = 0; row < LEVEL_HEIGHT; ++row) {
		for (int col = 0; col < LEVEL_WIDTH; ++col) {
			std::cout << grid[row * LEVEL_WIDTH + col] << " ";
		}
		std::cout << "\n";
	}

	BombFormation formation;
	
	std::vector<sf::Vector2i> frontier = { start };
	std::cout << "frontier is " << start.x << ", " << start.y << "\n";
	std::unordered_set<sf::Vector2i, Vector2iHash> visited;
	visited.emplace(start);
	formation.addBomb(start);
	do {		
		const auto cur = frontier.back();
		frontier.pop_back();
		for (auto dir = (int)lif::Direction::UP; dir < lif::Direction::NONE; ++dir) {
			for (int len = 1; len <= 2; ++len) {
				auto tile = lif::towards<int>(cur, static_cast<lif::Direction>(dir), len);
				if (tile.x < 0 || tile.y < 0) 
					continue;
    		if (visited.find(tile) != visited.end())
    			continue;
				if (grid[tile.y * LEVEL_WIDTH + tile.x] == '*') {
					// add bomb to frontier
      		visited.emplace(tile);
					frontier.push_back(tile);
					formation.addBomb(tile);
				}
			}
		}
	} while (!frontier.empty());

	if (formation.tiles.size() > 1) {
		// remove old formations
		if (formation.tiles.size() > 2) {
			for (auto it = formations.begin(); it != formations.end(); ) {
				bool found = false;
				for (const auto& tile : it->tiles) {
					if (std::find(formation.tiles.begin(), formation.tiles.end(), tile) != formation.tiles.end()) {
						it = formations.erase(it);
						found = true;
						break;
					}
				}
				if (!found)
					++it;
			}
		}
		formations.push_back(formation);
		std::cout << formation.signature << "\n";

		for (std::size_t i = 0; i < std::size(bombPatterns); ++i) {
			if (std::abs(bombPatterns[i].second - formation.signature) < 1e-6f) {
				std::cout << "added " << bombPatterns[i].first << "\n";
				break;
			}
		}
	}
}

void lif::BombFormationsManager::debugDisplay() const {
	int formIdx = 0;
	for (const auto& formation : formations) {
		static const sf::Color colors[] = { sf::Color::Red, sf::Color::Yellow, sf::Color::Blue, sf::Color::Green };
		for (auto tile : formation.tiles)
				lif::debugPainter->addCircleAt(sf::Vector2f(tile + sf::Vector2i { 1, 1 }) * float(lif::TILE_SIZE), lif::TILE_SIZE * 0.5, 
				                               colors[formIdx % std::size(colors)]);
		++formIdx;
	}
}
