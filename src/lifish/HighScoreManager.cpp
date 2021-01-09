#include "HighScoreManager.hpp"
#include "core.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>

using lif::HighScoreManager;

void HighScoreManager::loadHighScores() {
	std::stringstream completePath;
	completePath << lif::getSaveDir() << lif::DIRSEP << HIGH_SCORES_FNAME;

	entries.clear();

	std::ifstream f(completePath.str());

	while (f && entries.size() < MAX_SCORES_NUM) {
		std::array<char, 64> linebuf;
		f.getline(linebuf.data(), linebuf.size() - 1);
		std::string line(linebuf.data());

		if (line.length() == 0) continue;

		const auto splitIdx = line.find(' ');
		if (splitIdx == std::string::npos) {
			std::cerr << "Malformed high scores line: " << line << "\n";
			continue;
		}

		HighScoreEntry entry;
		entry.name = line.substr(0, splitIdx);

		const auto& scoreStr = line.substr(splitIdx);
		try {
			entry.score = static_cast<unsigned>(std::stoi(scoreStr));
		} catch (const std::invalid_argument&) {
			std::cerr << "Malformed high scores line: " << line << "\n";
			continue;
		}

		entries.push_back(entry);
	}

	std::sort(entries.begin(), entries.end(), [] (const auto& a, const auto& b) { return a.score > b.score; });
}

void HighScoreManager::saveHighScores() {
	std::stringstream completePath;
	completePath << lif::getSaveDir() << lif::DIRSEP << HIGH_SCORES_FNAME;

	std::ofstream f(completePath.str());
	if (!f) return;

	for (const auto& entry : entries) {
		f << entry.name << " " << entry.score << "\n";
	}
}

bool HighScoreManager::isHighScore(int score) const {
	return entries.size() < MAX_SCORES_NUM || entries[entries.size() - 1].score < static_cast<unsigned>(score);
}
