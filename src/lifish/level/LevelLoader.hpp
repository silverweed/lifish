#pragma once

#include <SFML/System/Vector2.hpp>

namespace lif {

class Level;
class LevelManager;

class LevelLoader {
public:
	/**
	 * Loads `level` into the LevelManager `lm`.
	 * Returns whether have been errors or not.
	 */
	static bool load(const lif::Level& level, lif::LevelManager& lm);
};

}
