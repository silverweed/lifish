#pragma once

#include <memory>
#include "Chronometer.hpp"

namespace Game {

class LevelManager;
class Level;

/**
 * This class handles the winning and losing conditions and switching
 * between levels.
 */
class WinLoseHandler final {
	enum class State {
		DEFAULT,
		HANDLING_WIN,
		HANDLING_LOSS
	} state = State::DEFAULT;
	
	Game::LevelManager& lm;
	sftools::Chronometer clock;
	bool levelClearSoundPlayed = false,
	     playerWinSoundPlayed = false;

	void _handleWin(std::unique_ptr<Game::Level>& level);
	void _handleLoss();
	void _checkCondition(std::unique_ptr<Game::Level>& lv);
public:
	explicit WinLoseHandler(Game::LevelManager& lm);

	/** In case of win or loss, performs the due actions (replaces the normal event loop). */
	void handleWinLose(std::unique_ptr<Game::Level>& level);
};

}
