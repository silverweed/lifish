#include "SaveManager.hpp"
#include "LevelManager.hpp"
#include "Options.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "LevelSet.hpp"
#include "json.hpp"
#include "Lifed.hpp"
#include "utils.hpp"
#include <iostream>

using lif::SaveManager;

bool SaveManager::saveGame(const std::string& filename, const lif::LevelManager& lm) {
	if (!lif::createDirIfNotExisting(lif::getSaveDir())) {
		return false;
	}

	std::ofstream saveFile(filename);

	nlohmann::json save;

	save["levelSet"] = lm.getLevel()->getLevelSet().getMeta("path");
	save["level"] = lm.getLevel()->getInfo().levelnum;
	save["nPlayers"] = lif::options.nPlayers;

	const auto& players = lm.players;
	for (unsigned i = 0; i < players.size(); ++i) {
		const auto& player = players[i];
		if (player == nullptr) {
			// Only save the score
			save["players"][i] = {
				{ "continues", -1 },
				{ "score", lm.getScore(i + 1) }
			};
			continue;
		}

		const auto& powers = player->getPowers();
		const auto& info = player->getInfo();
		save["players"][i] = {
			{ "continues", lm.getPlayerContinues(i + 1) },
			{ "remainingLives", info.remainingLives },
			{ "life", player->get<lif::Lifed>()->getLife() },
			{ "powers",
				{
					{ "bombFuseTime",   powers.bombFuseTime },
					{ "bombRadius",     powers.bombRadius },
					{ "maxBombs",       powers.maxBombs },
				}
			},
			{ "score", lm.getScore(i + 1) }
		};

		// Letters
		for (unsigned j = 0; j < lif::conf::player::N_EXTRA_LETTERS; ++j)
			save["players"][i]["extra"][j] = info.extra[j];
	}

	saveFile << save;

	return true;
}

lif::SaveData SaveManager::loadGame(const std::string& filename) {
	lif::SaveData data;
	try {
		nlohmann::json load = nlohmann::json::parse(std::ifstream(filename));

		data.levelSet = load["levelSet"].get<std::string>();
		data.level = load["level"];
		data.nPlayers = load["nPlayers"];
		for (unsigned i = 0; i < data.players.size(); ++i) {
			auto& player = data.players[i];
			const auto& pldata = load["players"][i];
			player.score = pldata["score"];
			player.continues = pldata["continues"];
			if (player.continues < 0)
				continue;

			player.remainingLives = pldata["remainingLives"];
			player.life = pldata["life"];
			const auto& powdata = pldata["powers"];
			player.powers.bombRadius = powdata["bombRadius"];
			player.powers.bombFuseTime = powdata["bombFuseTime"];
			player.powers.maxBombs = powdata["maxBombs"];
			const auto& exdata = pldata["extra"];
			for (unsigned j = 0; j < player.letters.size(); ++j)
				player.letters[j] = exdata[j];
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return data;
}
