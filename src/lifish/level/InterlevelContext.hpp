#pragma once

#include "WindowContext.hpp"
#include "game.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <array>

namespace lif {

class LevelManager;
class SidePanel;

/**
 * WindowContext handling level advancing and game over prompts
 */
class InterlevelContext : public lif::WindowContext {

	constexpr static unsigned MAX_HIGH_SCORE_NAME_LEN = 16;

	enum class State {
		DISTRIBUTING_POINTS,
		WAIT_DISTRIBUTING_POINTS,
		PROMPT_HIGHSCORE,
		PROMPT_CONTINUE,
		GETTING_READY
	} state = State::DISTRIBUTING_POINTS;

	enum PromptFlags {
		PROMPT_NONE = 0,
		PROMPT_HIGHSCORE = 1 << 0,
		PROMPT_REVIVE = 1 << 1,
	};

	sf::Time lastTickTime;
	sf::Time time;
	sf::Font interlevelFont;

	sf::Text centralText;
	sf::Text subtitleText;    // this is right under `centralText`
	sf::Text subsubtitleText; // this is under `subtitleText`
	sf::Text yesText, noText;

	lif::LevelManager& lm;
	const lif::SidePanel& sidePanel;

	sf::Time bonusTime = sf::Time::Zero;
	int bonusPoints = 0;
	/** Whether player (i+1) needs to be prompted for continue or not */
	std::array<char, lif::MAX_PLAYERS> mustPromptPlayer;
	unsigned short curPromptedPlayer = 0;
	/** Whether player is selecting 'yes' or 'no' during PROMPT_CONTINUE */
	bool yesSelected = true;
	bool retryingLevel = false;
	bool anyNewHighScores = false;

	std::array<char, MAX_HIGH_SCORE_NAME_LEN> buffer;
	size_t bufIdx = 0;
	sf::Text *bufferText = nullptr;

	sf::RectangleShape cursor;
	sf::Clock cursorClock;
	bool cursorVisible = true;
	sf::Vector2f charBounds;

	/** @return true if player chose to continue, else false */
	//bool _displayContinue(sf::RenderWindow& target, const lif::SidePanel& panel, short playerId);
	//void _displayGetReady(sf::RenderWindow& target, const lif::SidePanel& panel, short lvnum);
	void _setGettingReady();
	void _givePoints(int amount);
	void _tickDistributePoints();
	void _tickWaitDistributePoints();
	void _tickGettingReady();
	// Returns true if both players are dead
	bool _calcPrompts();
	void _setNextPrompt();
	void _tickPromptContinue();
	/** Sets up the window for prompting player i+1 for continue */
	void _preparePromptContinue(unsigned short i);
	void _ackPromptResponse();
	void _continueSelectYes();
	void _continueSelectNo();
	void _preparePromptHighScore(unsigned short i);
	bool _handleEventPromptContinue(sf::Event event);
	bool _handleEventPromptHighscore(sf::Event event);
	void _updateCursorPosition();

public:
	explicit InterlevelContext(lif::LevelManager& lm, const lif::SidePanel& sidePanel);

	void setGettingReady(unsigned short lvnum);
	void setAdvancingLevel();
	void setRetryingLevel();

	void update() override;
	bool handleEvent(sf::Window& window, sf::Event evt) override;
	void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
};

}
