#pragma once

#include "HomeScreen.hpp"

namespace Game {

/**
 * Singleton class for the about screen
 */
class AboutScreen : public Game::Screen {
	/** Private constructor */
	AboutScreen();
public:
	static AboutScreen& getInstance() {
		static AboutScreen instance;
		return instance;
	}

	std::set<Game::Screen*> getParents() override {
		return { &Game::HomeScreen::getInstance() };
	}
};

}
