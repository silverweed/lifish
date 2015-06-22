#pragma once
/**
 * Class containing a track's data
 */

#include <string>

namespace Game {

class Track {
	std::string name;
	float loopstart, looplength;
public:
	Track(const std::string& name, float start, float length);
	Track(unsigned short num, float start, float length);

	std::string getName() const { return name; }
	float getLoopStart() const { return loopstart; }
	float getLoopLength() const { return looplength; }
	float getLoopEnd() const { return loopstart + looplength; }
};

}
