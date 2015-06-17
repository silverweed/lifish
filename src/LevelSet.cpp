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
	for (auto& key : AVAIL_METADATA) {
		if (levelJSON.find(key) != levelJSON.end())
			metadata[key] = levelJSON[key].get<std::string>();
	}

	auto levelsdata = levelJSON["levels"];

	unsigned short lvnum = 1;
	/* lvinfo = {
	 *		"time": uint,
	 *		"tileset": ushort,
	 *		"tilemap": string
	 * }
	 */
	for (auto lvinfo : levelsdata) {
		Level *level = new Level(this);
		level->setTime((unsigned int)lvinfo["time"]);
		level->setTileset((unsigned short)lvinfo["tileset"]);
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
		  << "Levels: " << levels.size() << "\n";
	for (auto& pair : metadata) {
		if (pair.first == "name") continue;
		std::cout << pair.first << ": " << pair.second << "\n";
	}
}
