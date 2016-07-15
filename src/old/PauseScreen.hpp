#pragma once

#include "HomeScreen.hpp"

namespace Game {

/**
 * Singleton class for the Pause screen
 */
class PauseScreen : public Game::Screen {
	PauseScreen();

public:
	static PauseScreen& getInstance() {
		static PauseScreen instance;
		return instance;
	}
};

}
