#pragma once

#include <utility>
#include <vector>
#include <array>
#include <cstdint>
#include <initializer_list>
#include "collision_layers.hpp"
#include "Direction.hpp"
#include "Component.hpp"

namespace Game {

class LevelManager;

class Sighted : public Game::Component {
	using PartSeenEntitiesList = std::vector<std::pair<const Game::Entity*, unsigned short>>;
	using TotSeenEntitiesList = std::array<
		PartSeenEntitiesList,
		static_cast<unsigned short>(Game::Direction::NONE)>;

	/** Bitmask for opaque layers (entities with no Collider are always transparent) */
	intmax_t opaqueMask = 0;

	// vision radius in number of tiles. Negative means infinite.
	short visionRadius;

	// all entities seen in each direction, and their distance (nearest first) 
	TotSeenEntitiesList seen;

	const Game::LevelManager *lm = nullptr;


	bool _isOpaque(Game::Layers::Layer layer) const;
	/** Fills seen[dir] with entities seen in that direction */
	void _fillLine(const Game::Direction dir);

public:
	explicit Sighted(Game::Entity& owner, short visionRadius = -1);

	const TotSeenEntitiesList& entitiesSeen() const { 
		return seen;
	}

	const PartSeenEntitiesList& entitiesSeen(Game::Direction dir) const { 
		return seen[static_cast<size_t>(dir)];
	}

	/** Sets all layers in `layers` as (`opaque` ? opaque : transparent) */
	void setOpaque(std::initializer_list<Game::Layers::Layer> layers, bool opaque = true);

	void setLevelManager(const Game::LevelManager *_lm) { lm = _lm; }
	const Game::LevelManager* getLevelManager() const { return lm; }

	void update() override;
};

}
