#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "WindowContext.hpp"

namespace Game {

class LevelManager;
class SidePanel;

/**
 * WindowContext handling level advancing and game over prompts
 */
class InterlevelContext : public Game::WindowContext {

	enum class State {
		DISTRIBUTING_POINTS,
		GETTING_READY,
		PROMPT_CONTINUE
	} state = State::DISTRIBUTING_POINTS;

	sf::Clock clock;
	sf::Time lastTickTime;
	sf::Font interlevelFont;

	Game::LevelManager& lm;
	const Game::SidePanel& sidePanel;

	sf::Text centralText;
	sf::Text subtitleText;	// this is right under `centralText`

	sf::Time bonusTime = sf::Time::Zero;
	int bonusPoints = 0;

	/** @return true if player chose to continue, else false */
	//bool _displayContinue(sf::RenderWindow& target, const Game::SidePanel& panel, short playerId);
	//void _displayGetReady(sf::RenderWindow& target, const Game::SidePanel& panel, short lvnum);
	void _givePoints(int amount);
	void _tickDistributePoints();
	void _setGettingReady();
	void _tickGettingReady();

public:
	explicit InterlevelContext(Game::LevelManager& lm, const Game::SidePanel& sidePanel);

	void setAdvancingLevel();

	void update() override;
	bool handleEvent(sf::Window& window, sf::Event evt) override;
	void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
};

}
