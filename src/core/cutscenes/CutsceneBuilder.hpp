#pragma once

#include <vector>
#include <string>
#include "Cutscene.hpp"

namespace lif {

class CutsceneBuilder final {
public:
	/** Reads from `assets/cutscenes/cutsceneName.json` and builds a series of Cutscenes
	 *  with the properties found therein. The number of Cutscenes is the same as the size of
	 *  the "scenes" vector inside the JSON.
	 */
	static std::vector<lif::Cutscene> fromJson(const std::string& cutsceneName);
};

} // end namespace lif
