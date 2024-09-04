#pragma once

#include "game.hpp"
#include <array>

namespace lif {

constexpr const char* PREFERENCES_SAVE_FILE_NAME = "lif_prefs.json";

/** Saves preferences to file */
void savePreferences(std::string fpath);

/** Loads preferences from file, if it exists. */
void loadPreferences(std::string fpath);

} // end namespace lif
