#pragma once

#include "Level.hpp"
#include "EntityGroup.hpp"

namespace Game {

namespace LevelLoader {
	/**
	 * Fills `entities` as defined by `level`'s tilemap. 
	 * Returns whether have been errors or not.
	 */
	static bool load(const Game::Level& level, Game::EntityGroup& entities);
};

}
