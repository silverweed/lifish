#pragma once

#include "Component.hpp"

namespace Game {

class LevelManager;

class Sighted : public Game::Component {
	// vision radius in number of tiles. Negative means infinite.
	short visionRadius;

	// first entity seen in each direction.
	std::array<const Game::Entity*, 4> seen;

	const Game::LevelManager *lm = nullptr;

public:
	explicit Sighted(Game::Entity *const owner, short visionRadius = -1);

	const std::array<const Game::Entity*, 4>& entitiesSeen() const { return seen; }
	const Game::Entity* entitySeen(Game::Direction dir) const { return seen[dir]; }

	void setLevelManager(const Game::LevelManager *_lm) { lm = _lm; }

	void update() override;
};

}
