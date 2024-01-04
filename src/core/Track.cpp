#include "Track.hpp"
#include "core.hpp"

lif::Track lif::getNthTrack(int num, float start, float length) {
	using lif::DIRSEP;
	std::stringstream ss;
	ss << lif::assetDir << "music" << DIRSEP << "music" << num << ".ogg";
	lif::Track track;
	track.name = ss.str();
	track.loopstart = start;
	track.looplength = length;
	track.loopend = start + length;
	return track;
}
