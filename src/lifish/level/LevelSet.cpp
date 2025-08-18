#include "LevelSet.hpp"
#include "utils.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "json.hpp"

using lif::LevelSet;
using lif::Level;

using namespace picojson;

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
	value levelJSON;
	auto err = picojson::parse(levelJSON, fileContent);
	if (!err.empty()) {
		throw std::logic_error(err);
	}

	const auto &levelData = levelJSON.get<object>();
	
	// load metadata
	for (const auto& key : AVAIL_METADATA) {
		if (auto it = levelData.find(key); it != levelData.end())
			metadata[key] = it->second.get<std::string>();
	}
	metadata["path"] = lif::toRelativePath(path);

	// load tracks data
	const auto& tracksdata = get_or<array>(levelData, "tracks");
	unsigned tracknum = 1;
	/* trackinfo = {
	 *	"loop": {
	 *		"start": float,
	 *		"end": float
	 *	}
	 * }
	 */
	for (const auto& trackinfo : tracksdata) {
		const auto &trackObj = trackinfo.get<object>();
		const auto& loop = trackObj.find("loop")->second.get<object>();
		const float loopstart = loop.find("start")->second.get<double>();
		float looplength = -1;
		const auto lenIt = loop.find("length");
		if (lenIt != loop.end()) {
			looplength = lenIt->second.get<double>();
		} else {
			float loopend = loop.find("end")->second.get<double>();
			looplength = loopend - loopstart;
		}
		tracks.emplace_back(getNthTrack(tracknum++, loopstart, looplength));
	}

	// load enemies data
	const auto& enemydata = levelData.find("enemies")->second.get<array>();
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
	for (const auto& enemyJson : enemydata) {
		const auto &enemyinfo = enemyJson.get<object>();
		enemies[enemynum].ai = enemyinfo.find("ai")->second.get<int64_t>();
		enemies[enemynum].speed = enemyinfo.find("speed")->second.get<double>();

		const auto& atk = enemyinfo.find("attack")->second.get<object>();
		const auto& atktype = atk.find("type")->second.get<array>();

		enemies[enemynum].attack.type = static_cast<lif::AttackType>(0);
		for (unsigned i = 0; i < atktype.size(); ++i) {
			AttackType type;
			const auto& at = atktype[i].get<std::string>();
			if (!lif::stringToAttackType(at, type))
				throw std::invalid_argument(at.c_str());

			enemies[enemynum].attack.type = AttackType(
					static_cast<unsigned>(enemies[enemynum].attack.type)
					| static_cast<unsigned>(type));
		}

		// Optional fields
		enemies[enemynum].attack.bulletId = get_or<int64_t>(atk, "id");
		enemies[enemynum].attack.contactDamage = get_or<int64_t>(atk, "contactDamage");
		enemies[enemynum].attack.fireRate = get_or<double>(atk, "fireRate");
		enemies[enemynum].attack.blockTime = sf::milliseconds(get_or<double>(atk, "blockTime"));

		// Find range: first search for `range` (in pixels); if not found, search `tileRange`.
		// If neither is found, set range to -1 (infinite).
		enemies[enemynum].attack.range = -1;
		bool range_found = false;
		auto it = atk.find("range");
		if (it != atk.end()) {
			enemies[enemynum].attack.range = it->second.get<double>();
			range_found = true;
		}
		if (!range_found) {
			it = atk.find("tileRange");
			if (it != atk.end()) {
				enemies[enemynum].attack.range = static_cast<float>(it->second.get<int64_t>() * lif::TILE_SIZE);
			}
		}
		++enemynum;
	}


	// load levels data
	const auto& levelsdata = levelData.find("levels")->second.get<array>();

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
	for (const auto& lvJson : levelsdata) {
		const auto &lvinfo = lvJson.get<object>();
		const auto &tileIDs = lvinfo.find("tileIDs")->second.get<object>();
		LevelInfo info;
		info.levelnum          = lvnum++;
		info.time              = lvinfo.find("time")->second.get<int64_t>();
		info.track             = tracks[lvinfo.find("music")->second.get<int64_t>()-1];
		info.tilemap           = lvinfo.find("tilemap")->second.get<std::string>();
		info.width             = lvinfo.find("width")->second.get<int64_t>();
		info.height            = lvinfo.find("height")->second.get<int64_t>();
		info.tileIDs.border    = tileIDs.find("border")->second.get<int64_t>();
		info.tileIDs.bg        = tileIDs.find("bg")->second.get<int64_t>();
		info.tileIDs.fixed     = tileIDs.find("fixed")->second.get<int64_t>();
		info.tileIDs.breakable = tileIDs.find("breakable")->second.get<int64_t>();
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
