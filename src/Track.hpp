#pragma once
/**
 * Class containing a track's data
 */

#include <string>
#include <sstream>

namespace Game {

struct Track {
	std::string name;
	float loopstart, loopend, looplength;
};

Track getNthTrack(unsigned short num, float start, float length);

}
