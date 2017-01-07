#pragma once

#include <unordered_map>
#include <functional>
#include "Screen.hpp"

namespace lif {

namespace ui {

extern std::unordered_map<std::string, lif::ui::Screen::Callback> screenCallbacks;
extern std::string screenCallbackArg;

}

}
