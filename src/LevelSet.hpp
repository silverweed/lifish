#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Level.hpp"
#include "Track.hpp"
#include "Game.hpp"

namespace Game {

class Level;

class LevelSet {
	std::vector<Game::Level*> levels;
	std::vector<Game::Track> tracks;
	std::unordered_map<std::string, std::string> metadata;
public:
	LevelSet(const std::string& jsonPath);
	~LevelSet();

	/** Gets i-th level (numbering starts from 1), initializing
	 *  it if necessary.
	 */
	Level* getLevel(unsigned short i) const;
	unsigned short getLevelsNum() const { return levels.size(); }

	void printInfo() const;
};

}
