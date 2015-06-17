#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include "Level.hpp"
#include "Game.hpp"

namespace Game {

class LevelSet {
	std::vector<Level*> levels;
	// TODO
	//std::unordered_set<basic_json> metadata;
public:
	LevelSet(const std::string& jsonPath);
	~LevelSet();

	/** Gets i-th level (numbering starts from 1), initializing
	 *  it if necessary.
	 */
	Level* getLevel(unsigned short i) const;
	unsigned short getLevelsNum() const { return levels.size(); }
};

}
