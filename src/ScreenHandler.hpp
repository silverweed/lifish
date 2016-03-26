#pragma once

#include <SFML/Graphics.hpp>
#include "HomeScreen.hpp"
#include "PreferencesScreen.hpp"
#include "ControlsScreen.hpp"
#include "AboutScreen.hpp"
#include "PauseScreen.hpp"

namespace Game {

/** Bitmask used for handleScreenEvents() */
enum ScreenType {
	HOME_SCREEN        = 1,
	PREFERENCES_SCREEN = 1 << 1,
	CONTROLS_SCREEN    = 1 << 2,
	ABOUT_SCREEN       = 1 << 3,
	PAUSE_SCREEN       = 1 << 4,
};

enum class Action {
	START_GAME,
	LOAD_GAME,
	SAVE_GAME,
	EXIT,
	DO_NOTHING
};

class ScreenHandler final {
	// All possible screens
	struct {
		Game::HomeScreen& home = Game::HomeScreen::getInstance();
		Game::PreferencesScreen& preferences = Game::PreferencesScreen::getInstance();
		Game::ControlsScreen& controls = Game::ControlsScreen::getInstance();
		Game::AboutScreen& about = Game::AboutScreen::getInstance();
		Game::PauseScreen& pause = Game::PauseScreen::getInstance();
	} screens;
public:
	/** Displays the menu, starting with `rootScreen` and exiting when it should change to a
	 *  screen which is not enabled. All screens are enabled by default.
	 */
	Game::Action handleScreenEvents(sf::RenderWindow& window, ScreenType rootScreen, int enabledScreens);
};

}
