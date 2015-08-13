#pragma once

#include <string>

/**
 * An object which has a sound associated with it. Note that
 * this class is only used for objects which have a single 
 * associated sound, not multiple ones.
 */
namespace Game {

class Sounded {
	std::string soundFile;
public:
	Sounded(const std::string& _soundFile) : soundFile(_soundFile) {}

	const std::string& getSoundFile() const { return soundFile; }
};

}
