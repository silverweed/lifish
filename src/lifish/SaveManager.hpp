#pragma once

#include <string>
#include <fstream>

namespace Game {

class LevelManager;

/**
 * Serializes the game state into JSON data or
 * deserializes those data from a file.
 * Save file format: {
 *     level: int,
 *     players: [
 *        {
 *            continues: int,
 *            remainingLives: int,
 *            life: int,
 *            powers: {
 *                bombfusetime: int,
 *                bombradius: int,
 *                maxbombs: int
 *            },
 *            letters: [ bool ],
 *            score: int
 *        }
 *    ]
 * }
 */
class SaveManager {
public:
	/** Saves the game state into `filename` */
	static bool saveGame(const std::string& filename, const Game::LevelManager& lr);

	/** Loads a game state saved in `filename` into `lr` and `start_level` */
	static bool loadGame(const std::string& filename,
			Game::LevelManager& lr, unsigned short& start_level);
};

}
