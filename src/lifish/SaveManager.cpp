#include "SaveManager.hpp"
#include "LevelManager.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "json.hpp"
#include "Lifed.hpp"
#include <iostream>

using lif::SaveManager;

bool SaveManager::saveGame(const std::string& filename, const lif::LevelManager& lr) {
	std::ofstream saveFile(filename);

	nlohmann::json save;

	// Current level
	save["level"] = lr.getLevel()->getInfo().levelnum;
	
	const auto& players = lr.players;
	for (unsigned i = 0; i < players.size(); ++i) {
		const auto& player = players[i];
		if (player == nullptr) {
			// Only save the score
			save["players"][i] = {
				{ "continues", 0 },
				{ "score", lif::score[i] }
			};
			continue;
		}

		const auto powers = player->getInfo().powers;
		const auto info = player->getInfo();
		save["players"][i] = {
			{ "continues", lif::playerContinues[i] },
			{ "remainingLives", info.remainingLives },
			{ "life", player->get<lif::Lifed>()->getLife() },
			{ "powers",
				{
					{ "bombFuseTime", powers.bombFuseTime.asMilliseconds() },
					{ "bombRadius",   powers.bombRadius },
					{ "maxBombs",     powers.maxBombs }
				}
			},
			{ "score", lif::score[i] }
		};

		// Letters
		for (unsigned j = 0; j < lif::conf::player::N_EXTRA_LETTERS; ++j)
			save["players"][i]["extra"][j] = info.extra[j];
	}

	saveFile << save;

	return true;
}

bool SaveManager::loadGame(const std::string& filename, 
		lif::LevelManager& lr, unsigned short& start_level)
{
	std::ifstream saveFile(filename);

	try {
		nlohmann::json load = nlohmann::json::parse(saveFile);
		
		start_level = load["level"];
		
		const auto& players = lr.players;
		for (unsigned i = 0; i < players.size(); ++i) {
			const auto& player = players[i];
			const auto pldata = load["players"][i];
			// Continues
			lif::playerContinues[i] = pldata["continues"];

			// Remaining Lives
			player->setRemainingLives(pldata["remainingLives"]);

			// Current health
			player->get<lif::Lifed>()->setLife(pldata["life"]);

			// Powers
			const auto powdata = pldata["powers"];
			/// Bomb fuse time (in ms)
			player->setBombFuseTime(sf::milliseconds(powdata["bombFuseTime"]));

			/// Bomb radius
			player->setBombRadius(powdata["bombRadius"]);

			/// Max bombs
			player->setMaxBombs(powdata["maxBombs"]);

			// Letters
			const auto exdata = pldata["extra"];
			for (unsigned j = 0; j < lif::conf::player::N_EXTRA_LETTERS; ++j)
				player->setExtra(j, pldata["extra"]);

			// Score
			lif::score[i] = pldata["score"];
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}
