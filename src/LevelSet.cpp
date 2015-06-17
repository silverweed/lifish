#include "LevelSet.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;
using Game::LevelSet;
using Game::Level;

LevelSet::LevelSet(const std::string& path) {
	json levelJSON = json::parse(std::ifstream(path.c_str()));
	// TODO: load metadata

	auto levelsdata = levelJSON["levels"];

	unsigned short lvnum = 1;
	/* lvinfo = {
	 *		"time": uint,
	 *		"tileset": ushort,
	 *		"tilemap": string
	 * }
	 */
	for (auto lvinfo : levelsdata) {
		// TODO: create the levels
		Level *level = new Level;
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
