#pragma once

#include "PreferencesScreen.hpp"

namespace Game {

class ControlsScreen : public Game::Screen {
	/** Whose player we're changing controls */
	unsigned short selectedPlayer = 1;


	void _highlightSelectedPlayer();
	void _changeJoystickBomb(sf::RenderWindow& window);

	ControlsScreen();
public:
	static ControlsScreen& getInstance() {
		static ControlsScreen instance;
		return instance;
	}

	Game::Screen* getParent() override {
		return &Game::PreferencesScreen::getInstance();
	}

	void selectPlayer(unsigned short id);
	unsigned short getSelectedPlayer() const { return selectedPlayer; }

	void changeControl(sf::RenderWindow& window, const std::string& textKey);

	/** Toggles the Game::useJoystick variable for the selected player */
	void toggleJoystick();

	void draw(sf::RenderTarget& window) override;
	void triggerMouseOver(const sf::Vector2f& mousePos) override;
};

}
