#include "LevelSet.hpp"
#include "json.hpp"
#include "utils.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <typeinfo>

using json = nlohmann::json;
using Game::LevelSet;
using Game::Level;

/** Optional additional metadata; all values must be strings */
static constexpr const char* AVAIL_METADATA[] = {
	"author", "created", "difficulty"
};

LevelSet::LevelSet(const std::string& path) {
	json levelJSON = json::parse(std::ifstream(path.c_str()));
	// load metadata
	metadata["name"] = levelJSON["name"].get<std::string>();
	if (metadata["name"].length() < 1)
		metadata["name"] = path;
	for (const auto& key : AVAIL_METADATA) {
		if (levelJSON.find(key) != levelJSON.end())
			metadata[key] = levelJSON[(std::string)key].get<std::string>();
	}

	// load tracks data
	const auto tracksdata = levelJSON["tracks"];
	unsigned short tracknum = 1;
	/* trackinfo = {
	 *	"loop": {
	 *		"start": float,
	 *		"end": float
	 *	}
	 * }
	 */
	for (const auto& trackinfo : tracksdata) {
		auto loop = trackinfo["loop"];
		float loopstart = loop["start"]; 
		float looplength = -1;
		auto len = loop.find("length");
		if (len != loop.end()) {
			looplength = *len;
		} else {
			float loopend = loop["end"];
			looplength = loopend - loopstart;
		}
		tracks.push_back(getNthTrack(tracknum++, loopstart, looplength));
	}

	// load enemies data
	const auto enemydata = levelJSON["enemies"];
	unsigned short enemynum = 0;
	/* enemyinfo = {
	 *	"ai": ushort,
	 *	"speed": ushort,
	 *	"attack": {
	 *		"type": string,
	 *		"damage": short,
	 *		"id": ushort        [opt]
	 *		"speed": float,     [opt]
	 *		"fireRate": float,  [opt]
	 *		"blockTime": float, [opt]
	 *		"range": short,     [opt, default=-1]
	 *	}
	 *
	 * }
	 */
	for (const auto& enemyinfo : enemydata) {
		enemies[enemynum].ai = enemyinfo["ai"];
		enemies[enemynum].speed = enemyinfo["speed"];
		
		auto atk = enemyinfo["attack"];
		auto atktype = atk["type"];
		
		auto fst = atktype[0].get<std::string>();
		if (!Game::stringToAttackType(fst, enemies[enemynum].attack.type))
			throw std::invalid_argument(fst.c_str());

		for (unsigned short i = 1; i < atktype.size(); ++i) {
			AttackType type;
			auto snd = atktype[1].get<std::string>();
			if (!Game::stringToAttackType(snd, type))
				throw std::invalid_argument(snd.c_str());

			enemies[enemynum].attack.type = AttackType(
					static_cast<unsigned int>(enemies[enemynum].attack.type) 
					| static_cast<unsigned int>(type));
		}

		// Mandatory fields
		enemies[enemynum].attack.damage   = atk["damage"].get<short>();
		// Optional fields
		auto it = atk.find("speed");
		if (it != atk.end())
			enemies[enemynum].attack.speed = it->get<float>();
		
		it = atk.find("id");
		if (it != atk.end())
			enemies[enemynum].attack.id = it->get<unsigned short>();

		it = atk.find("fireRate");
		if (it != atk.end())
			enemies[enemynum].attack.fireRate = it->get<float>();

		it = atk.find("range");
		enemies[enemynum].attack.range = it != atk.end() ? it->get<short>() : -1;

		++enemynum;
	}


	// load levels data
	const auto levelsdata = levelJSON["levels"];

	unsigned short lvnum = 1;
	/* lvinfo = {
	 *	"time": uint,
	 *	"tilemap": string,
	 *	"music": ushort,
	 *	"tileIDs": {
	 *		"border": ushort,
	 *		"bg": ushort,
	 *		"fixed": ushort,
	 *		"breakable": ushort
	 *	}
	 * }
	 */
	for (const auto& lvinfo : levelsdata) {
		LevelInfo info;
		info.levelnum          = lvnum;
		info.time              = lvinfo["time"].get<unsigned int>();
		info.track             = tracks[lvinfo["music"].get<unsigned short>()-1];
		info.tilemap           = lvinfo["tilemap"];
		info.tileIDs.border    = lvinfo["tileIDs"]["border"].get<unsigned short>();
		info.tileIDs.bg        = lvinfo["tileIDs"]["bg"].get<unsigned short>();
		info.tileIDs.fixed     = lvinfo["tileIDs"]["fixed"].get<unsigned short>();
		info.tileIDs.breakable = lvinfo["tileIDs"]["breakable"].get<unsigned short>();
		levels.push_back(info);
	}
}

Level* LevelSet::getLevel(unsigned short num) const {
	if (num == 0 || num > levels.size()) return nullptr;
	Level *lv = new Level(this);
	lv->levelInfo = levels[num - 1];
	if (!lv->init()) {
		delete lv;
		return nullptr;
	}
	return lv;
}

std::string LevelSet::toString() const {
	std::stringstream ss;
	ss << "Level Set: " << metadata.find("name")->second << "\r\n"
	   << "Tracks: " << tracks.size() << "\r\n"
	   << "Levels: " << levels.size() << "\r\n";
	for (const auto& pair : metadata) {
		if (pair.first == "name") continue;
		ss << pair.first << ": " << pair.second << "\r\n";
	}
	return ss.str();
}
