#pragma once

#include <unordered_map>
#include <functional>

namespace Game {

namespace UI {

enum class Action {
	START_GAME,
	LOAD_GAME,
	SAVE_GAME,
	SWITCH_SCREEN,
	SWITCH_TO_PARENT,
	EXIT,
	DO_NOTHING
};

using ScreenCallback = std::function<Game::UI::Action()>;

extern std::unordered_map<std::string, ScreenCallback> screenCallbacks;
extern std::string screenCallbackArg;

}

}
