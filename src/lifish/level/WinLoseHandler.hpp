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
public:
	enum class State {
		DEFAULT,
		HANDLING_WIN,
		HANDLING_LOSS,
		ADVANCING_LEVEL
	};
private:
	State state = State::DEFAULT;
	
	Game::LevelManager& lm;
	sftools::Chronometer clock;
	bool levelClearSoundPlayed = false,
	     playerWinSoundPlayed = false;

	void _handleWin();
	void _handleLoss();
	void _checkCondition();
public:
	explicit WinLoseHandler(Game::LevelManager& lm);

	/** In case of win or loss, performs the due actions (replaces the normal event loop).
	 *  @return the new state of WinLoseHandler.
	 */
	State handleWinLose();
};

}
