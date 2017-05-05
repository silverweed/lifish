#pragma once

#include <string>
#include <fstream>
#include <array>
#include "game.hpp"
#include "conf/player.hpp"
#include "Player.hpp"

namespace lif {

class LevelManager;

struct PlayerSaveData {
	int continues;
	int remainingLives;
	int life;
	lif::PlayerPowers powers;
	std::array<bool, lif::conf::player::N_EXTRA_LETTERS> letters;
	unsigned score;
};

struct SaveData {
	std::string levelSet;
	unsigned short level;
	std::array<PlayerSaveData, lif::MAX_PLAYERS> players;
};

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
 *                maxBombs: int,
 *                incendiaryBomb: bool,
 *                throwableBomb: bool,
 *                absorb: int,
 *                armor: int,
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

	/** Loads a game state saved in `filename` into `lm` and `start_level`.
	 *  Save data validation is NOT performed by this method.
	 */
	static lif::SaveData loadGame(const std::string& filename);
};

}
