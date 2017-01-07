#pragma once

#include <unordered_map>
#include <functional>
#include "Screen.hpp"

namespace lif {

namespace UI {

extern std::unordered_map<std::string, lif::UI::Screen::Callback> screenCallbacks;
extern std::string screenCallbackArg;

}

}
