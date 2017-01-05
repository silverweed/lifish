#include "screen_callbacks.hpp"
#include "dialogs.hpp"

using ScreenCallback = Game::UI::Screen::Callback;
using Game::UI::Action;

std::string Game::UI::screenCallbackArg;

static Action cb_exit() { return Action::EXIT; }
static Action cb_back() { return Action::SWITCH_TO_PARENT; }
static Action cb_start() { return Action::START_GAME; }
static Action cb_resume() { return Action::DEACTIVATE_UI; }
static ScreenCallback cb_switchTo(const std::string& screen) {
	return [screen] () {
		Game::UI::screenCallbackArg = screen;
		return Action::SWITCH_SCREEN_OVERRIDE_PARENT;
	};
}
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
	{ "start", cb_start },
	{ "about", cb_switchTo("about") },
	{ "load", cb_load },
	{ "home", cb_switchTo("home") },
	{ "preferences", cb_switchTo("preferences") },
	{ "controls", cb_switchTo("controls") },
	{ "resume", cb_resume }
};
