#pragma once

namespace Game {

class LevelManager;

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

	void _handleWin();
	void _handleLoss();
	void _checkCondition();
public:
	explicit WinLoseHandler(Game::LevelManager& lm);

	/** In case of win or loss, performs the due actions (replaces the normal event loop). */
	void handleWinLose();
};

}
