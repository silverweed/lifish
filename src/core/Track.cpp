#include "Track.hpp"
#include "core.hpp"

lif::Track lif::getNthTrack(int num, float start, float length) {
	using lif::DIRSEP;
	std::stringstream ss;
#if defined(SFML_SYSTEM_MACOS)
	ss << lif::assetDir << "music" << DIRSEP << "music" << num << ".ogg";
#else
	ss << lif::pwd << DIRSEP << "assets" << DIRSEP << "music" << DIRSEP << "music" << num << ".ogg";
#endif
	lif::Track track;
	track.name = ss.str();
	track.loopstart = start;
	track.looplength = length;
	track.loopend = start + length;
	return track;
}
