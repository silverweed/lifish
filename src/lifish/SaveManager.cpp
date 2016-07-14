#include "SaveManager.hpp"
#include "LevelManager.hpp"
#include "json.hpp"
#include "Lifed.hpp"
#include <iostream>

using Game::SaveManager;

bool SaveManager::saveGame(const std::string& filename, const Game::LevelManager& lr) {
	std::ofstream saveFile(filename);

	nlohmann::json save;

	// Current level
	save["level"] = lr.getLevel()->getInfo().levelnum;
	
	const auto& players = lr.players;
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

		const auto powers = player->getInfo().powers;
		const auto info = player->getInfo();
		save["players"][i] = {
			{ "continues", Game::playerContinues[i] },
			{ "remainingLives", info.remainingLives },
			{ "life", player->get<Game::Lifed>()->getLife() },
			{ "powers",
				{
					{ "bombFuseTime", powers.bombFuseTime.asMilliseconds() },
					{ "bombRadius",   powers.bombRadius },
					{ "maxBombs",     powers.maxBombs }
				}
			},
			{ "score", Game::score[i] }
		};

		// Letters
		for (unsigned short j = 0; j < Game::Conf::Player::N_EXTRA_LETTERS; ++j)
			save["players"][i]["extra"][j] = info.extra[j];
	}

	saveFile << save;

	return true;
}

bool SaveManager::loadGame(const std::string& filename, 
		Game::LevelManager& lr, unsigned short& start_level)
{
	std::ifstream saveFile(filename);

	try {
		nlohmann::json load = nlohmann::json::parse(saveFile);
		
		start_level = load["level"];
		
		const auto& players = lr.players;
		for (unsigned short i = 0; i < players.size(); ++i) {
			const auto& player = players[i];
			const auto pldata = load["players"][i];
			// Continues
			Game::playerContinues[i] = pldata["continues"];

			// Remaining Lives
			player->setRemainingLives(pldata["remainingLives"]);

			// Current health
			player->get<Game::Lifed>()->setLife(pldata["life"]);

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
			for (unsigned short j = 0; j < Game::Conf::Player::N_EXTRA_LETTERS; ++j)
				player->setExtra(j, pldata["extra"]);

			// Score
			Game::score[i] = pldata["score"];
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}
