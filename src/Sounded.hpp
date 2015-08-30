#pragma once

#include <string>
#include <vector>

/**
 * An object which has one or more sounds associated with it. 
 */
namespace Game {

class Sounded {
	std::vector<std::string> soundFiles;
public:
	Sounded(std::initializer_list<std::string> _soundFiles) {
		for (auto& s : _soundFiles)
			soundFiles.push_back(s);
	}

	virtual const std::string& getSoundFile(unsigned short n = 0) const { return soundFiles[n]; }
};

}
