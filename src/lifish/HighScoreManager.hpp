#pragma once

#include <string>
#include <vector>

namespace lif {

struct HighScoreEntry {
	std::string name;
	unsigned score;
};

class HighScoreManager {
	static constexpr const char* HIGH_SCORES_FNAME = ".highscores.txt";
	static constexpr std::size_t MAX_SCORES_NUM = 20;

	std::vector<HighScoreEntry> entries;

public:
	const std::vector<HighScoreEntry> getSortedHighScores() const { return entries; }

	void saveHighScores();
	void loadHighScores();

	bool isHighScore(int score) const;
};

}
