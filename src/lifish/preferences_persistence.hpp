#pragma once

#include "game.hpp"
#include <array>

namespace lif {

#if defined(SFML_SYSTEM_MACOS)
// Preferences file shouldn't be hidden
constexpr const char* PREFERENCES_SAVE_FILE_NAME = "lif_prefs.json";
#else
constexpr const char* PREFERENCES_SAVE_FILE_NAME = ".lif_prefs.json";
#endif

/** Saves preferences to file */
void savePreferences(std::string fpath);

/** Loads preferences from file, if it exists. */
void loadPreferences(std::string fpath);

} // end namespace lif
