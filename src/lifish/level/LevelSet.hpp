#pragma once

#include "Enemy.hpp"
#include "Level.hpp"
#include "Stringable.hpp"
#include "Track.hpp"
#include <SFML/System/NonCopyable.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace lif {

/**
 * A LevelSet groups information about a set of Levels, along with
 * extra info about enemies, music, etc. Basically, it holds the
 * customizable game configuration read from a JSON file.
 */
class LevelSet final : public lif::Stringable, private sf::NonCopyable {
	std::vector<lif::LevelInfo> levels;
	std::vector<lif::Track> tracks;
	std::array<EnemyInfo, lif::N_ENEMIES> enemies;
	std::unordered_map<std::string, std::string> metadata;

public:
	LevelSet() {}
	LevelSet(const std::string& jsonPath);
	~LevelSet() {}

	bool loadFromFile(const std::string& jsonPath);

	/** Constructs the i-th level (starting from 1) and returns it if init() is successful. */
	std::unique_ptr<Level> getLevel(unsigned i) const;
	unsigned short getLevelsNum() const { return levels.size(); }
	std::string getMeta(const std::string& key) const;
	const EnemyInfo& getEnemyInfo(const int id) const { return enemies[id - 1]; }

	std::string toString() const override;
};

}
