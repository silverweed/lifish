#pragma once

#include "game.hpp"
#include <array>

namespace lif {

constexpr const char* PREFERENCES_SAVE_FILE_NAME = ".lif_prefs.json";

/** Saves preferences to file */
void savePreferences(const char *fname);

/** Loads preferences from file, if it exists. */
void loadPreferences(const char *fname);

} // end namespace lif
