#include "SaveManager.hpp"
#include "LevelManager.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "LevelSet.hpp"
#include "json.hpp"
#include "Lifed.hpp"
#include <iostream>

using lif::SaveManager;

bool SaveManager::saveGame(const std::string& filename, const lif::LevelManager& lr) {
	std::ofstream saveFile(filename);

	nlohmann::json save;

	// Current levelset
	save["levelSet"] = lr.getLevel()->getLevelSet().getMeta("name");
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

lif::SaveData SaveManager::loadGame(const std::string& filename) {
	SaveData data;
	std::ifstream saveFile(filename);

	try {
		nlohmann::json load = nlohmann::json::parse(saveFile);
		
		data.levelSet = load["levelSet"];
		data.level = load["level"];
		for (unsigned i = 0; i < data.players.size(); ++i) {
			auto& player = data.players[i];
			const auto& pldata = load["players"][i];
			player.continues = pldata["continues"];
			player.remainingLives = pldata["remainingLives"];
			player.life = pldata["life"];
			player.score = pldata["score"];
			const auto& powdata = pldata["powers"];
			player.powers.bombRadius = powdata["bombRadius"];
			player.powers.bombFuseTime = powdata["bombFuseTime"];
			player.powers.maxBombs = powdata["maxBombs"];
			const auto& exdata = pldata["extra"];
			for (unsigned j = 0; j < player.letters.size(); ++j)
				player.letters[j] = exdata[j];
		}
		// TODO add validation
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return data;
}
