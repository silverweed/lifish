#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <SFML/System/NonCopyable.hpp>
#include "Level.hpp"
#include "Track.hpp"
#include "Game.hpp"
#include "Stringable.hpp"

namespace Game {

struct EnemyInfo {
	unsigned short ai = 0;
	unsigned short speed = 0;
};

/**
 * A LevelSet groups information about a set of Levels, along with
 * extra info about enemies, music, etc. Basically, it holds the
 * customizable game configuration read from a JSON file.
 */
class LevelSet final : public Game::Stringable, private sf::NonCopyable {
	std::vector<Game::LevelInfo> levels;
	std::vector<Game::Track> tracks;
	std::array<EnemyInfo, Game::N_ENEMIES> enemies;
	std::unordered_map<std::string, std::string> metadata;

public:
	LevelSet(const std::string& jsonPath);
	~LevelSet();

	/** Constructs the i-th level and returns it if init() is successful.
	 *  Callee MUST take care of deleting the returned Level.
	 */
	Level* getLevel(unsigned short i) const;
	unsigned short getLevelsNum() const { return levels.size(); }

	const EnemyInfo& getEnemyInfo(const unsigned short id) const { return enemies[id-1]; }

	std::string toString() const override;
};

}
