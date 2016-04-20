#pragma once

#include <string>
#include <vector>
#include "Component.hpp"

/**
 * An object which has one or more sounds associated with it. 
 */
namespace Game {

class Sounded : public Game::Component {
	std::vector<std::string> soundFiles;
public:
	explicit Sounded(Game::Entity *const owner, std::initializer_list<std::string> _soundFiles) 
		: Game::Component(owner)
	{
		for (auto& s : _soundFiles)
			soundFiles.push_back(s);
	}

	const std::string& getSoundFile(unsigned short n = 0) const { return soundFiles[n]; }
};

}
