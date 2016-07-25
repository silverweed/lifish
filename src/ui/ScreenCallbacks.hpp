#pragma once

#include <unordered_map>
#include <functional>
#include "Screen.hpp"

namespace Game {

namespace UI {

extern std::unordered_map<std::string, Game::UI::Screen::Callback> screenCallbacks;
extern std::string screenCallbackArg;

}

}
