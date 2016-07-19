#include "ScreenCallbacks.hpp"
#include "dialogs.hpp"

using Game::UI::ScreenCallback;
using Game::UI::Action;

std::string Game::UI::screenCallbackArg;

static Action cb_exit() { return Action::EXIT; }
static Action cb_back() { return Action::SWITCH_TO_PARENT; }
static Action cb_start() { return Action::START_GAME; }
static Action cb_about() { return Game::UI::screenCallbackArg = "about", Action::SWITCH_SCREEN; }
static Action cb_preferences() { return Game::UI::screenCallbackArg = "preferences", Action::SWITCH_SCREEN; }
static Action cb_controls() { return Game::UI::screenCallbackArg = "controls", Action::SWITCH_SCREEN; }
static Action cb_load() { 
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
	{ "exit", cb_exit },
	{ "back", cb_back },
	{ "home:start", cb_start },
	{ "home:about", cb_about },
	{ "home:load", cb_load },
	{ "home:preferences", cb_preferences },
	{ "preferences:controls", cb_controls }
};
