#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "Chronometer.hpp"

namespace Game {

class LevelManager;
class Level;
class SidePanel;

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
		GAME_WON
	};
private:
	State state = State::DEFAULT;
	
	Game::LevelManager& lm;
	sftools::Chronometer clock;
	bool levelClearSoundPlayed = false,
	     playerWinSoundPlayed = false;
	sf::Font interlevelFont;

	void _handleWin();
	void _handleLoss();
	void _checkCondition();
	/** @return true if player chose to continue, else false */
	bool _displayContinue(sf::RenderWindow& target, const Game::SidePanel& panel, short playerId);
	void _displayGetReady(sf::RenderWindow& target, const Game::SidePanel& panel, short lvnum);
public:
	explicit WinLoseHandler(Game::LevelManager& lm);

	/** In case of win or loss, performs the due actions (replaces the normal event loop).
	 *  @return the new state of WinLoseHandler.
	 */
	void handleWinLose();

	State getState() const { return state; }

	void advanceLevel(sf::RenderWindow& target, const Game::SidePanel& panel);
};

}
