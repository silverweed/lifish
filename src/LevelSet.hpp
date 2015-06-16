#pragma once

#include <string>
#include <vector>
#include "Level.hpp"
#include "Game.hpp"
#include "json.hpp"

namespace Game {

class LevelSet {
	std::vector<Level> levels;
public:
	static LevelSet loadFromJSON(const std::string& path);
};

}
