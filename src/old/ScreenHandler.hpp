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

class ScreenHandler final : public Game::WithOrigin, public sf::Drawable {
	// All possible screens
	struct Screens {
		Game::HomeScreen& home;
		Game::PreferencesScreen& preferences;
		Game::ControlsScreen& controls;
		Game::AboutScreen& about;
		Game::PauseScreen& pause;

		Screens()
			: home(Game::HomeScreen::getInstance()) 
			, preferences(Game::PreferencesScreen::getInstance())
			, controls(Game::ControlsScreen::getInstance())
			, about(Game::AboutScreen::getInstance())
			, pause(Game::PauseScreen::getInstance())
		{}
	} screens;

	Game::Screen *curScreen = nullptr;
	
	ScreenHandler() : screens(), curScreen(&screens.home) {}

public:
	static ScreenHandler& getInstance() {
		static ScreenHandler instance;
		return instance;
	}

	/** Displays the menu, starting with `rootScreen` and exiting when it should change to a
	 *  screen which is not enabled. All screens are enabled by default.
	 */
	Game::Action handleScreenEvents(sf::RenderWindow& window, ScreenType rootScreen, int enabledScreens);

	void draw(sf::RenderTarget& target, sf::RenderStates) const override;

	void setOrigin(const sf::Vector2f& pos) override {
		Game::WithOrigin::setOrigin(pos);
		screens.home.setOrigin(pos);
		screens.preferences.setOrigin(pos);
		screens.controls.setOrigin(pos);
		screens.about.setOrigin(pos);
		screens.pause.setOrigin(pos);
	}
};

}
