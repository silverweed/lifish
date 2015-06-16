#include "LevelSet.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;
using namespace Game;

LevelSet LevelSet::loadFromJSON(const std::string& path) {
	LevelSet levelSet;
	json levelJSON = json::parse(std::ifstream(path.c_str()));
	auto levels = levelJSON["levels"];

	std::cerr << "levels: " << levels << std::endl;
	unsigned short x = 0, y = 0;
	for (auto lvinfo : levels) {
		// TODO: create the levels
	}

	return levelSet;
}
