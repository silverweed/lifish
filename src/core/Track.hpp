#pragma once

#include <string>
#include <sstream>

namespace lif {

/**
 * Struct containing a track's data
 */
struct Track {
	std::string name;
	float loopstart = 0,
	      loopend = 0,
	      looplength = 0;
};

Track getNthTrack(int num, float start, float length);

}
