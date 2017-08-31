#pragma once

#include <SFML/System/Vector2.hpp>

namespace lif {

class Level;
class LevelManager;
class Teleport;
class EntityGroup;

class LevelLoader {

	static void _addTorch(lif::EntityGroup& entities, const sf::Vector2f& curPos,
			const lif::Level& level, int left, int top);
public:
	/**
	 * Loads `level` into the LevelManager `lm`.
	 * Returns whether have been errors or not.
	 */
	static bool load(const lif::Level& level, lif::LevelManager& lm);
};

}
