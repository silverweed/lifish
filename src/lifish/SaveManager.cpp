#include "SaveManager.hpp"
#include "LevelManager.hpp"
#include "Options.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "LevelSet.hpp"
#include "Lifed.hpp"
#include "utils.hpp"
#include <iostream>
#include <tinyjson.h>

using lif::SaveManager;

bool SaveManager::saveGame(const std::string& filename, const lif::LevelManager& lm) {
	if (!lif::createDirIfNotExisting(lif::getSaveDir())) {
		return false;
	}

	std::ofstream saveFile(filename);

	tinyjson::json save = tinyjson::json_object {};

	save.add_member("levelSet", std::string(lm.getLevel()->getLevelSet().getMeta("path")));
	save.add_member("level", lm.getLevel()->getInfo().levelnum);
	save.add_member("nPlayers", lif::options.nPlayers);

	const auto& players = lm.players;
	save.add_member("players", tinyjson::json_array {});
	for (unsigned i = 0; i < players.size(); ++i) {
		const auto& player = players[i];
		if (player == nullptr) {
			// Only save the score
			save["players"].add_element(tinyjson::json_object {
				{ "continues", -1 },
				{ "score", lm.getScore(i + 1) }
			});
			continue;
		}

		const auto& powers = player->getPowers();
		const auto& info = player->getInfo();
		save["players"].add_element(tinyjson::json_object {
			{ "continues", lm.getPlayerContinues(i + 1) },
			{ "remainingLives", info.remainingLives },
			{ "life", player->get<lif::Lifed>()->getLife() },
			{ "powers",
				tinyjson::json_object {{
					{ "bombFuseTime",   powers.bombFuseTime.asSeconds() },
					{ "bombRadius",     powers.bombRadius },
					{ "maxBombs",       powers.maxBombs }
				}}
			},
			{ "score", lm.getScore(i + 1) }
		});

		// Letters
		save["players"][i].add_member("extra", tinyjson::json_array {});
		for (unsigned j = 0; j < lif::conf::player::N_EXTRA_LETTERS; ++j)
			save["players"][i]["extra"].add_element(info.extra[j]);
	}

	saveFile << save.to_string();

	return true;
}

lif::SaveData SaveManager::loadGame(const std::string& filename) {
	lif::SaveData data;
	try {
		std::string fileContent = lif::readEntireFile(filename);
		tinyjson::json load = tinyjson::parser::parse(fileContent.c_str());

		data.levelSet = load["levelSet"].get_string();
		data.level = load["level"].get_integer();
		data.nPlayers = load["nPlayers"].get_integer();
		for (unsigned i = 0; i < data.players.size(); ++i) {
			auto& player = data.players[i];
			const auto& pldata = load["players"][i];
			player.score = pldata["score"].get_integer();
			player.continues = pldata["continues"].get_integer();
			if (player.continues < 0)
				continue;

			player.remainingLives = pldata["remainingLives"].get_integer();
			player.life = pldata["life"].get_integer();
			const auto& powdata = pldata["powers"];
			player.powers.bombRadius = powdata["bombRadius"].get_integer();
			player.powers.bombFuseTime = sf::seconds(powdata["bombFuseTime"].get_double());
			player.powers.maxBombs = powdata["maxBombs"].get_integer();
			const auto& exdata = pldata["extra"];
			for (unsigned j = 0; j < player.letters.size(); ++j)
				player.letters[j] = exdata[j].get_bool();
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return data;
}
