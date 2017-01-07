#pragma once

#include <string>
#include <unordered_map>
#include "Component.hpp"

/**
 * An object which has one or more sounds associated with it. 
 */
namespace lif {

class Sounded : public lif::Component {
	std::unordered_map<std::string, std::string> soundFiles;
public:
	explicit Sounded(lif::Entity& owner, std::initializer_list<std::pair<std::string, std::string>> _soundFiles)
		: lif::Component(owner)
	{
		for (auto& s : _soundFiles)
			soundFiles[s.first] = s.second;
	}

	std::string getSoundFile(std::string name) const {
		auto it = soundFiles.find(name);
		if (it == soundFiles.end()) return "";
		return it->second;
	}

	void setSoundFile(std::string name, std::string file) {
		soundFiles[name] = file;
	}
};

}
