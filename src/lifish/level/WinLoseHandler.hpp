#pragma once

#include "InterlevelContext.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

namespace lif {

class LevelManager;
class Level;
class SidePanel;
class WindowContext;
#ifndef RELEASE
namespace debug {
	class DebugEventHandler;
}
#endif

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
		RETRY_LEVEL,
		ADVANCED_LEVEL,
		GAME_WON,
		EXIT_GAME
	};

private:

#ifndef RELEASE
	friend class lif::debug::DebugEventHandler;
#endif

	State state = State::DEFAULT;

	lif::LevelManager& lm;
	lif::InterlevelContext interlevelCtx;
	sf::Time time;
	bool levelClearSoundPlayed = false,
	     playerWinSoundPlayed = false;

	void _handleWin();
	void _handleLoss();
	void _checkCondition();
public:
	explicit WinLoseHandler(lif::LevelManager& lm, const lif::SidePanel& sidePanel);

	lif::WindowContext& getInterlevelContext() { return interlevelCtx; }

	/** In case of win or loss, performs the due actions */
	void handleWinLose();
	State getState() const { return state; }
	//void advanceLevel(sf::RenderWindow& target, const lif::SidePanel& panel);
};

}
