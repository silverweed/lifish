#include "SaveManager.hpp"
#include "json.hpp"
#include <iostream>

using Game::SaveManager;

bool SaveManager::saveGame(const std::string& filename, const Game::LevelRenderer& lr) {
	std::ofstream saveFile(filename);

	nlohmann::json save;

	// Current level
	save["level"] = lr.getLevel()->getLevelNum();
	
	const auto players = lr._players;
	for (unsigned short i = 0; i < players.size(); ++i) {
		const auto& player = players[i];
		if (player == nullptr) {
			// Only save the score
			save["players"][i] = {
				{ "continues", 0 },
				{ "score", Game::score[i] }
			};
			continue;
		}

		save["players"][i] = {
			{ "continues", Game::playerContinues[i] },
			{ "remainingLives", player->getRemainingLives() },
			{ "life", player->getLife() },
			{ "powers",
				{
					{ "bombFuseTime", player->powers.bombFuseTime },
					{ "bombRadius", player->powers.bombRadius },
					{ "maxBombs", player->powers.maxBombs }
				}
			},
			{ "score", Game::score[i] }
		};

		// Letters
		for (unsigned short j = 0; j < Game::N_EXTRA_LETTERS; ++j)
			save["players"][i]["extra"][j] = player->extra[j];
	}

	saveFile << save;

	return true;
}

bool SaveManager::loadGame(const std::string& filename, 
		Game::LevelRenderer& lr, unsigned short& start_level)
{
	std::ifstream saveFile(filename, std::ofstream::binary);

	try {
		nlohmann::json load = nlohmann::json::parse(saveFile);
		
		start_level = load["level"];
		
		const auto players = lr._players;
		for (unsigned short i = 0; i < players.size(); ++i) {
			const auto& player = players[i];
			const auto pldata = load["players"][i];
			// Continues
			Game::playerContinues[i] = pldata["continues"];

			if (Game::playerContinues[i] == 0) {
				Game::score[i] = pldata["score"];
				lr.removePlayer(i+1, false);
				continue;
			}

			// Remaining Lives
			player->setRemainingLives(pldata["remainingLives"]);

			// Current health
			player->setLife(pldata["life"]);

			// Powers
			const auto powdata = pldata["powers"];
			/// Bomb fuse time (in ms)
			player->powers.bombFuseTime = powdata["bombFuseTime"];

			/// Bomb radius
			player->powers.bombRadius = powdata["bombRadius"];

			/// Max bombs
			player->powers.maxBombs = powdata["maxBombs"];

			// Letters
			const auto exdata = pldata["extra"];
			for (unsigned short j = 0; j < Game::N_EXTRA_LETTERS; ++j)
				player->extra[j] = bool(exdata[j]);

			// Score
			Game::score[i] = pldata["score"];
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}
