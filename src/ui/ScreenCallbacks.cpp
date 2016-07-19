#include "ScreenCallbacks.hpp"
#include "dialogs.hpp"

using Game::UI::ScreenCallback;
using Game::UI::Action;

std::string Game::UI::screenCallbackArg;

static Action g_exit() { return Action::EXIT; }
static Action g_back() { return Action::SWITCH_TO_PARENT; }
static Action home_start() { return Action::START_GAME; }
static Action home_about() { return Game::UI::screenCallbackArg = "about", Action::SWITCH_SCREEN; }
static Action home_load() { 
	const auto fname = Game::display_load_dialog();
	if (fname.length() > 0) {
		// TODO
		//Game::LevelManager lr {
			//new Game::Player(sf::Vector2f(0, 0), 1),
			//new Game::Player(sf::Vector2f(0, 0), 2)
		//};

		//if (Game::SaveManager::loadGame(fname, lr, start_level))
			//play_game(window, levelSetName, lr, start_level);
		//else
			//std::cerr << "Couldn't load game from " << fname 
				  //<< ": the save file is probably corrupt." << std::endl;
	}
	return Action::DO_NOTHING;
}

std::unordered_map<std::string, ScreenCallback> Game::UI::screenCallbacks = {
	{ "exit", g_exit },
	{ "back", g_back },
	{ "home:start", home_start },
	{ "home:about", home_about },
	{ "home:load", home_load },
};
