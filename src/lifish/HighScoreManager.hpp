#pragma once

#include <SFML/Config.hpp>
#include <string>
#include <vector>

namespace lif {

struct HighScoreEntry {
	std::string name;
	int score;
};

class HighScoreManager {
	static constexpr const char* HIGH_SCORES_FNAME = "highscores.txt";
	static constexpr std::size_t MAX_SCORES_NUM = 16;

	std::vector<HighScoreEntry> entries;

public:
	const std::vector<HighScoreEntry> getSortedHighScores() const { return entries; }

	void saveHighScores();
	void loadHighScores();

	bool isHighScore(int score) const;

	void addHighScore(const HighScoreEntry& entry);
};

}
