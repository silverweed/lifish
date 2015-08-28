#pragma once

#include "Screen.hpp"

namespace Game {

/**
 * Singleton class for the home screen
 */
class HomeScreen : public Game::Screen {
	/** Private constructor */
	HomeScreen();
public:
	static HomeScreen& getInstance() {
		static HomeScreen instance;
		return instance;
	}
};

}
