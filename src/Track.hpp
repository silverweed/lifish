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

static Track getNthTrack(unsigned short num, float start, float length) {
	std::stringstream ss;
	ss << Game::pwd << DIRSEP << "assets" << DIRSEP << "music" << DIRSEP << "music" << num << ".ogg";
	Track track;
	track.name = ss.str();
	track.loopstart = start;
	track.looplength = length;
	track.loopend = start + length;
	return track;
}

}
