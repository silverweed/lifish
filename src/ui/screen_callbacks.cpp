#include "screen_callbacks.hpp"

using ScreenCallback = lif::ui::Screen::Callback;
using lif::ui::Action;

std::string lif::ui::screenCallbackArg;

static Action cb_exit()   { return Action::EXIT; }
static Action cb_back()   { return Action::SWITCH_TO_PARENT; }
static Action cb_start()  { return Action::START_GAME; }
static Action cb_resume() { return Action::DEACTIVATE_UI; }
static Action cb_quit()   { return Action::QUIT_TO_HOME; }
static ScreenCallback cb_switchTo(const std::string& screen) {
	return [screen] () {
		lif::ui::screenCallbackArg = screen;
		return Action::SWITCH_SCREEN_OVERRIDE_PARENT;
	};
}

std::unordered_map<std::string, ScreenCallback> lif::ui::screenCallbacks = {
	{ "exit",        cb_exit },
	{ "back",        cb_back },
	{ "start",       cb_start },
	{ "about",       cb_switchTo("about") },
	{ "save",        cb_switchTo("save") },
	{ "load",        cb_switchTo("load") },
	{ "home",        cb_switchTo("home") },
	{ "quit",        cb_quit },
	{ "preferences", cb_switchTo("preferences") },
	{ "controls",    cb_switchTo("controls") },
	{ "resume",      cb_resume }
};
