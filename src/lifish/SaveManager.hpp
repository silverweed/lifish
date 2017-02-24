#pragma once

#include <string>
#include <fstream>

namespace lif {

class LevelManager;

/**
 * Serializes the game state into JSON data or
 * deserializes those data from a file.
 * Save file format: {
 *     levelSet: string,
 *     level: int,
 *     players: [
 *        {
 *            continues: int,
 *            remainingLives: int,
 *            life: int,
 *            powers: {
 *                bombFuseTime: int,
 *                bombRadius: int,
 *                maxBombs: int
 *            },
 *            letters: [ bool ],
 *            score: int
 *        }
 *    ]
 * }
 */
class SaveManager {
public:
	SaveManager() = delete;

	/** Saves the game state into `filename` */
	static bool saveGame(const std::string& filename, const lif::LevelManager& lm);

	/** Loads a game state saved in `filename` into `lm` and `start_level` */
	// TODO: change this to return something more generic than modifying lm directly
	static bool loadGame(const std::string& filename,
			lif::LevelManager& lm, unsigned short& start_level);
};

}
