#include "SaveManager.hpp"
#include "LevelManager.hpp"
#include "Options.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "LevelSet.hpp"
#include "Lifed.hpp"
#include "utils.hpp"
#include <iostream>
#include "json.hpp"

using lif::SaveManager;

using namespace picojson;

bool SaveManager::saveGame(const std::string& filename, const lif::LevelManager& lm) {
	if (!lif::createDirIfNotExisting(lif::getSaveDir())) {
		return false;
	}

	std::ofstream saveFile(filename);

	auto save = value::object {};

	save["levelSet"] = value(std::string(lm.getLevel()->getLevelSet().getMeta("path")));
	save["level"] = value(int64_t(lm.getLevel()->getInfo().levelnum));
	save["nPlayers"] = value(int64_t(lif::options.nPlayers));

	const auto& players = lm.players;
	save["players"] = value(value::array {});
	for (unsigned i = 0; i < players.size(); ++i) {
		const auto& player = players[i];
		auto &playersArr = save["players"].get<array>();
		if (player == nullptr) {
			// Only save the score
			playersArr.push_back(value(value::object {
				{ "continues", value(int64_t(-1)) },
				{ "score", value(int64_t(lm.getScore(i + 1))) }
			}));
			continue;
		}

		const auto& powers = player->getPowers();
		const auto& info = player->getInfo();

		// Letters
		auto extra = value::array {};
		for (unsigned j = 0; j < lif::conf::player::N_EXTRA_LETTERS; ++j)
			extra.push_back(value(info.extra[j]));

		playersArr.push_back(value(value::object {
			{ "continues", value(int64_t(lm.getPlayerContinues(i + 1))) },
			{ "remainingLives", value(int64_t(info.remainingLives)) },
			{ "life", value(int64_t(player->get<lif::Lifed>()->getLife())) },
			{ "powers",
				value(value::object {{
					{ "bombFuseTime",   value(double(powers.bombFuseTime.asSeconds())) },
					{ "bombRadius",     value(int64_t(powers.bombRadius)) },
					{ "maxBombs",       value(int64_t(powers.maxBombs)) }
				}})
			},
			{ "score", value(int64_t(lm.getScore(i + 1))) },
			{ "extra", value(extra) }
		}));
	}

	saveFile << value(save).serialize();

	return true;
}

lif::SaveData SaveManager::loadGame(const std::string& filename) {
	lif::SaveData data;
	try {
		std::string fileContent = lif::readEntireFile(filename);
		value loadJson;
		auto err = picojson::parse(loadJson, fileContent);
		if (!err.empty())
			throw std::logic_error(err);

		const auto &load = loadJson.get<object>();
		data.levelSet = get_or<std::string>(load, "levelSet");
		data.level = get_or<int64_t>(load, "level");
		data.nPlayers = get_or<int64_t>(load, "nPlayers");
		const auto &playersdata = get_or<array>(load, "players");
		for (unsigned i = 0; i < std::min(data.players.size(), playersdata.size()); ++i) {
			auto& player = data.players[i];
			const auto &pldata = playersdata[i].get<object>();
			player.score = get_or<int64_t>(pldata, "score");
			player.continues = get_or<int64_t>(pldata, "continues");
			if (player.continues < 0)
				continue;

			player.remainingLives = get_or<int64_t>(pldata, "remainingLives");
			player.life = get_or<int64_t>(pldata, "life");
			const auto& powdata = pldata.find("powers")->second.get<object>();
			player.powers.bombRadius = get_or<int64_t>(powdata, "bombRadius");
			player.powers.bombFuseTime = sf::seconds(get_or<double>(powdata, "bombFuseTime"));
			player.powers.maxBombs = get_or<int64_t>(powdata, "maxBombs");
			const auto& exdata = get_or<array>(pldata, "extra");
			for (unsigned j = 0; j < player.letters.size(); ++j)
				player.letters[j] = exdata[j].get<bool>();
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return data;
}
