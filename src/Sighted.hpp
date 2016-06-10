#pragma once

#include <utility>
#include <array>
#include "Direction.hpp"
#include "Component.hpp"

namespace Game {

class LevelManager;

class Sighted : public Game::Component {
	// vision radius in number of tiles. Negative means infinite.
	short visionRadius;

	// first entity seen in each direction, and its distance
	std::array<std::pair<const Game::Entity*, unsigned short>, 4> seen;

	const Game::LevelManager *lm = nullptr;


	std::pair<const Game::Entity*, unsigned short> _fillLine(
			const Game::LevelManager *lm, const Game::Direction dir);

public:
	explicit Sighted(Game::Entity *const owner, short visionRadius = -1);

	const std::array<std::pair<const Game::Entity*, unsigned short>, 4>& entitiesSeen() const { 
		return seen;
	}
	std::pair<const Game::Entity*, unsigned short> entitySeen(Game::Direction dir) const { 
		return seen[static_cast<size_t>(dir)];
	}

	void setLevelManager(const Game::LevelManager *_lm) { lm = _lm; }

	void update() override;
};

}
