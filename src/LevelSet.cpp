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
			metadata[key] = levelJSON[key].get<std::string>();
	}

	// load tracks data
	auto tracksdata = levelJSON["tracks"];
	unsigned short tracknum = 1;
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

	// load levels data
	auto levelsdata = levelJSON["levels"];

	unsigned short lvnum = 1;
	/* lvinfo = {
	 *		"time": uint,
	 *		"tileset": ushort,
	 *		"tilemap": string,
	 *		"music": ushort,
	 * }
	 */
	for (const auto& lvinfo : levelsdata) {
		Level *level = new Level(this);
		level->setTime((unsigned int)lvinfo["time"]);
		level->setTileset((unsigned short)lvinfo["tileset"]);
		level->setTrack(tracks[(unsigned short)lvinfo["music"]-1]);
		level->setLevelNum(lvnum);
		if (!level->setTilemap(lvinfo["tilemap"]))
			std::cerr << "[LevelSet.cpp] Level " << lvnum << " has invalid tilemap: skipping." << std::endl;
		else {
			++lvnum;
			levels.push_back(level);
		}
	}
}

LevelSet::~LevelSet() {
	for (auto& it : levels) 
		if (it != nullptr)
			delete it;
}

Level* LevelSet::getLevel(unsigned short num) const {
	if (num > levels.size()) return nullptr;
	if (!levels[num-1]->isInitialized())
		levels[num-1]->init();
	return levels[num-1];
}

void LevelSet::printInfo() const {
	std::cout << "Level Set: " << metadata.find("name")->second << "\n"
		  << "Tracks: " << tracks.size() << "\n"
		  << "Levels: " << levels.size() << "\n";
	for (const auto& pair : metadata) {
		if (pair.first == "name") continue;
		std::cout << pair.first << ": " << pair.second << "\n";
	}
}
