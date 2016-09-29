#pragma once

namespace Game {

class Level;
class LevelManager;

class LevelLoader {
public:
	/**
	 * Loads `level` into the LevelManager `lm`.
	 * Returns whether have been errors or not.
	 */
	static bool load(const Game::Level& level, Game::LevelManager& lm);
};

}
