#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "Chronometer.hpp"
#include "InterlevelContext.hpp"

namespace Game {

class LevelManager;
class Level;
class SidePanel;
class WindowContext;

/**
 * This class handles the winning and losing conditions and switching
 * between levels.
 * TODO: make this compatible with multithreading
 */
class WinLoseHandler final {
public:
	enum class State {
		DEFAULT,
		HANDLING_WIN,
		HANDLING_LOSS,
		ADVANCING_LEVEL,
		ADVANCED_LEVEL,
		GAME_WON
	};

private:
	State state = State::DEFAULT;
	
	Game::LevelManager& lm;
	Game::InterlevelContext interlevelCtx;
	sftools::Chronometer clock;
	bool levelClearSoundPlayed = false,
	     playerWinSoundPlayed = false;

	void _handleWin();
	void _handleLoss();
	void _checkCondition();
public:
	explicit WinLoseHandler(Game::LevelManager& lm, const Game::SidePanel& sidePanel);

	Game::WindowContext& getInterlevelContext() { return interlevelCtx; }

	/** In case of win or loss, performs the due actions */
	void handleWinLose();
	State getState() const { return state; }
	//void advanceLevel(sf::RenderWindow& target, const Game::SidePanel& panel);
};

}
