#include "LevelSet.hpp"
#include "json.hpp"
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
	 *	"speed": ushort
	 * }
	 */
	for (const auto& enemyinfo : enemydata) {
		enemies[enemynum].ai = enemyinfo["ai"];
		enemies[enemynum].speed = enemyinfo["speed"];
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
		info.time              = (unsigned int)lvinfo["time"];
		info.track             = tracks[(unsigned short)lvinfo["music"]-1];
		info.tilemap           = lvinfo["tilemap"];
		info.tileIDs.border    = (unsigned short)lvinfo["tileIDs"]["border"];
		info.tileIDs.bg        = (unsigned short)lvinfo["tileIDs"]["bg"];
		info.tileIDs.fixed     = (unsigned short)lvinfo["tileIDs"]["fixed"];
		info.tileIDs.breakable = (unsigned short)lvinfo["tileIDs"]["breakable"];
		levels.push_back(info);
	}
}

Level* LevelSet::getLevel(unsigned short num) const {
	if (num == 0 || num > levels.size()) return nullptr;
	Level *lv = new Level(this);
	lv.levelInfo = levels[num - 1];
	if (!lv.init()) {
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
