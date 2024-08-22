#include "screen_callbacks.hpp"
#include "preferences_persistence.hpp"
#include "language.hpp"

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
static ScreenCallback cb_lang(lif::Language lang) {
	return [lang] () {
		lif::switchLanguage(lang);
		lif::savePreferences(lif::preferencesPath);
		return Action::DO_NOTHING;
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
	{ "high_scores", cb_switchTo("high_score") },
	{ "controls",    cb_switchTo("controls") },
	{ "resume",      cb_resume },
	{ "lang_en",     cb_lang(lif::Language::ENGLISH) },
	{ "lang_it",     cb_lang(lif::Language::ITALIAN) },
	{ "lang_de",     cb_lang(lif::Language::GERMAN) },
	{ "lang_fr",     cb_lang(lif::Language::FRENCH) }
};
