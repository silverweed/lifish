#pragma once

#include "Screen.hpp"
#include "controls.hpp"

namespace Game {

namespace UI {

class ControlsScreen : public Game::UI::Screen {
	/** Whose player we're changing controls */
	unsigned short selectedPlayer = 1;
	/** The control we're changing right now, if any */
	Game::ShadedText *changingCtrlText = nullptr;

	void _highlightSelectedPlayer();
	void _resyncCommandStrings();

	// Callbacks
	/** Selects `id`-th player (starting from 1) */
	Game::UI::Action _selectPlayer(unsigned short id);
	/** Sets the `textKey` text to be in 'changing' state */
	Game::UI::Action _changeControl(const std::string& textKey);
	/** Toggles the Game::useJoystick variable for the selected player */
	Game::UI::Action _toggleJoystick();

public:
	explicit ControlsScreen(const sf::RenderWindow& window);

	void update() override;
	bool receiveEvent(const sf::Event& event) override;
};

}

}
