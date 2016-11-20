#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "WindowContext.hpp"
#include "game.hpp"

namespace Game {

class LevelManager;
class SidePanel;

/**
 * WindowContext handling level advancing and game over prompts
 */
class InterlevelContext : public Game::WindowContext {

	enum class State {
		DISTRIBUTING_POINTS,
		WAIT_DISTRIBUTING_POINTS,
		PROMPT_CONTINUE,
		GETTING_READY
	} state = State::DISTRIBUTING_POINTS;

	sf::Clock clock;
	sf::Time lastTickTime;
	sf::Font interlevelFont;

	sf::Text centralText;
	sf::Text subtitleText;    // this is right under `centralText`
	sf::Text subsubtitleText; // this is under `subtitleText`
	sf::Text yesText, noText;

	Game::LevelManager& lm;
	const Game::SidePanel& sidePanel;

	sf::Time bonusTime = sf::Time::Zero;
	int bonusPoints = 0;
	/** Whether player (i+1) needs to be prompted for continue or not */
	std::array<bool, Game::MAX_PLAYERS> mustPromptPlayer;
	unsigned short curPromptedPlayer;
	/** Whether player is selecting 'yes' or 'no' during PROMPT_CONTINUE */
	bool yesSelected;

	/** @return true if player chose to continue, else false */
	//bool _displayContinue(sf::RenderWindow& target, const Game::SidePanel& panel, short playerId);
	//void _displayGetReady(sf::RenderWindow& target, const Game::SidePanel& panel, short lvnum);
	void _givePoints(int amount);
	void _tickDistributePoints();
	void _tickWaitDistributePoints();
	void _setGettingReady();
	void _tickGettingReady();
	void _setPromptContinue();
	void _tickPromptContinue();
	/** Sets up the window for prompting player i+1 for continue */
	void _preparePromptContinue(unsigned short i);
	void _ackPromptResponse();

public:
	explicit InterlevelContext(Game::LevelManager& lm, const Game::SidePanel& sidePanel);

	void setAdvancingLevel();

	void update() override;
	bool handleEvent(sf::Window& window, sf::Event evt) override;
	void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
};

}
