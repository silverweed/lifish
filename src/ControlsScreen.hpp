#pragma once

#include "PreferencesScreen.hpp"

namespace Game {

class ControlsScreen : public Game::Screen {
	/** Whose player we're changing controls */
	unsigned short selectedPlayer = 1;


	void _highlightSelectedPlayer();

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

	void changeControl(sf::RenderWindow& window, const std::string& textKey);

	void draw(sf::RenderTarget& window) override;
	void triggerMouseOver(const sf::Vector2f& mousePos) override;
};

}
