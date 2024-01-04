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
#if defined(SFML_SYSTEM_MACOS)
	// For convenience, make the high scores file visible to the end user on macOS
	static constexpr const char* HIGH_SCORES_FNAME = "highscores.txt";
#else
	static constexpr const char* HIGH_SCORES_FNAME = ".highscores.txt";
#endif
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
