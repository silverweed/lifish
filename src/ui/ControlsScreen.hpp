#pragma once

#include "Screen.hpp"

namespace Game {

namespace UI {

class ControlsScreen : public Game::UI::Screen {
	/** Whose player we're changing controls */
	unsigned short selectedPlayer = 1;


/*	void _highlightSelectedPlayer();
	void _changeJoystickBomb(sf::RenderWindow& window);

	void selectPlayer(unsigned short id);
	unsigned short getSelectedPlayer() const { return selectedPlayer; }

	void changeControl(sf::RenderWindow& window, const std::string& textKey);*/

	/** Toggles the Game::useJoystick variable for the selected player */
/*	void toggleJoystick();

	void triggerMouseOver(const sf::Vector2f& mousePos) override;
*/
public:
	ControlsScreen();
};

}

}
