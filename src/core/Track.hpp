#pragma once

#include <string>
#include <sstream>

namespace lif {

/**
 * Struct containing a track's data
 */
struct Track {
	std::string name;
	float loopstart, loopend, looplength;
};

Track getNthTrack(int num, float start, float length);

}
