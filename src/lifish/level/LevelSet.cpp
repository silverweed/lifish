#include "LevelSet.hpp"
#include "utils.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <tinyjson.h>

using lif::LevelSet;
using lif::Level;

/** Optional additional metadata; all values must be strings */
static constexpr const char* AVAIL_METADATA[] = {
	"author", "created", "difficulty", "comment", "name"
};

LevelSet::LevelSet(const std::string& path) {
	try {
		loadFromFile(path);
	} catch (const std::exception& e) {
		std::cerr << "[ FATAL ] Failed to parse levelset: " << e.what() << "\n";
		std::exit(1);
	}
}

bool LevelSet::loadFromFile(const std::string& path) {
	{
		// Check file exists
		std::ifstream file(path);
		if (!file.good())
			return false;
	}

	std::string fileContent = lif::readEntireFile(path);
	tinyjson::json levelJSON = tinyjson::parser::parse(fileContent.c_str());

	// load metadata
	for (const auto& key : AVAIL_METADATA) {
		if (auto it = levelJSON.find(key))
			metadata[key] = it->get_string();
	}
	metadata["path"] = lif::toRelativePath(path);

	// load tracks data
	const auto& tracksdata = levelJSON["tracks"];
	unsigned tracknum = 1;
	/* trackinfo = {
	 *	"loop": {
	 *		"start": float,
	 *		"end": float
	 *	}
	 * }
	 */
	for (const auto& trackinfo : tracksdata.get_array()) {
		const auto& loop = trackinfo["loop"];
		const float loopstart = loop["start"].get_double();
		float looplength = -1;
		const auto len = loop.find("length");
		if (len) {
			looplength = len->get_double();
		} else {
			float loopend = loop["end"].get_double();
			looplength = loopend - loopstart;
		}
		tracks.emplace_back(getNthTrack(tracknum++, loopstart, looplength));
	}

	// load enemies data
	const auto& enemydata = levelJSON["enemies"];
	unsigned enemynum = 0;
	/* enemyinfo = {
	 *	"ai": uint,
	 *	"speed": uint,
	 *	"attack": {
	 *		"type": [string],
	 *		"id": uint           [opt]
	 *		"contactDamage": int [opt]
	 *		"fireRate": float,   [opt]
	 *		"blockTime": float,  [opt]
	 *		"range": float,      [opt, default=-1]
	 *		"tileRange": short,  [opt, only valid if `range` was not specified]
	 *	}
	 *
	 * }
	 */
	for (const auto& enemyinfo : enemydata.get_array()) {
		enemies[enemynum].ai = enemyinfo["ai"].get_integer();
		enemies[enemynum].speed = enemyinfo["speed"].get_double();

		const auto& atk = enemyinfo["attack"];
		const auto& atktype = atk["type"];

		enemies[enemynum].attack.type = static_cast<lif::AttackType>(0);
		for (unsigned i = 0; i < atktype.size(); ++i) {
			AttackType type;
			const auto& at = atktype[i].get_string();
			if (!lif::stringToAttackType(at, type))
				throw std::invalid_argument(at.c_str());

			enemies[enemynum].attack.type = AttackType(
					static_cast<unsigned>(enemies[enemynum].attack.type)
					| static_cast<unsigned>(type));
		}

		// Optional fields
		auto it = atk.find("id");
		if (it)
			enemies[enemynum].attack.bulletId = it->get_integer();

		it = atk.find("contactDamage");
		if (it)
			enemies[enemynum].attack.contactDamage = it->get_integer();

		it = atk.find("fireRate");
		if (it)
			enemies[enemynum].attack.fireRate = it->get_double();

		it = atk.find("blockTime");
		if (it)
			enemies[enemynum].attack.blockTime = sf::milliseconds(it->get_double());

		// Find range: first search for `range` (in pixels); if not found, search `tileRange`.
		// If neither is found, set range to -1 (infinite).
		enemies[enemynum].attack.range = -1;
		bool range_found = false;
		it = atk.find("range");
		if (it) {
			enemies[enemynum].attack.range = it->get_double();
			range_found = true;
		}
		if (!range_found) {
			it = atk.find("tileRange");
			if (it) {
				enemies[enemynum].attack.range = static_cast<float>(it->get_integer() * lif::TILE_SIZE);
			}
		}
		++enemynum;
	}


	// load levels data
	const auto& levelsdata = levelJSON["levels"];

	unsigned lvnum = 1;
	/* lvinfo = {
	 *	"time": uint,
	 *	"tilemap": string,
	 *	"music": uint,
	 *	"width": uint,
	 *	"height": uint,
	 *	"tileIDs": {
	 *		"border": uint,
	 *		"bg": uint,
	 *		"fixed": uint,
	 *		"breakable": uint
	 *	},
	 *	"effects": [string] [opt]
	 *	"cutscenePre": string [opt]
	 *	"cutscenePost": string [opt]
	 * }
	 */
	for (const auto& lvinfo : levelsdata.get_array()) {
		LevelInfo info;
		info.levelnum          = lvnum++;
		info.time              = lvinfo["time"].get_integer();
		info.track             = tracks[lvinfo["music"].get_integer()-1];
		info.tilemap           = lvinfo["tilemap"].get_string();
		info.width             = lvinfo["width"].get_integer();
		info.height            = lvinfo["height"].get_integer();
		info.tileIDs.border    = lvinfo["tileIDs"]["border"].get_integer();
		info.tileIDs.bg        = lvinfo["tileIDs"]["bg"].get_integer();
		info.tileIDs.fixed     = lvinfo["tileIDs"]["fixed"].get_integer();
		info.tileIDs.breakable = lvinfo["tileIDs"]["breakable"].get_integer();
		levels.emplace_back(info);
	}

	return true;
}

std::unique_ptr<Level> LevelSet::getLevel(unsigned num) const {
	std::unique_ptr<Level> level;
	if (num > 0 && num <= levels.size()) {
		level = std::make_unique<Level>(*this);
		level->info = levels[num - 1];
		if (!level->init()) {
			std::cerr << "WARNING: level " << num << " failed to initialize!" << std::endl;
			level.reset();
		}
	}
	return level;
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

std::string LevelSet::getMeta(const std::string& key) const {
	auto val = metadata.find(key);
	if (val == metadata.end()) return "";
	return val->second;
}
