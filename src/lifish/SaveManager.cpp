#include "SaveManager.hpp"
#include "LevelManager.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "LevelSet.hpp"
#include "json.hpp"
#include "Lifed.hpp"
#include "utils.hpp"
#include <iostream>

using lif::SaveManager;

bool SaveManager::saveGame(const std::string& filename, const lif::LevelManager& lm) {
	std::ofstream saveFile(filename);

	nlohmann::json save;

	// Current levelset
	save["levelSet"] = lm.getLevel()->getLevelSet().getMeta("path");
	// Current level
	save["level"] = lm.getLevel()->getInfo().levelnum;
	
	const auto& players = lm.players;
	for (unsigned i = 0; i < players.size(); ++i) {
		const auto& player = players[i];
		if (player == nullptr) {
			// Only save the score
			save["players"][i] = {
				{ "continues", 0 },
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
					{ "incendiaryBomb", powers.incendiaryBomb },
					{ "throwableBomb",  powers.throwableBomb },
					{ "absorb",         powers.absorb },
					{ "armor",          powers.armor },
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
			player.powers.incendiaryBomb = powdata["incendiaryBomb"];
			player.powers.throwableBomb = powdata["throwableBomb"];
			player.powers.absorb = powdata["absorb"];
			player.powers.armor = powdata["armor"];
			const auto& exdata = pldata["extra"];
			for (unsigned j = 0; j < player.letters.size(); ++j)
				player.letters[j] = exdata[j];
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return data;
}
