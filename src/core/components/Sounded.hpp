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
	using SoundList = std::initializer_list<std::pair<std::string, std::string>>;

	explicit Sounded(lif::Entity& owner, SoundList _soundFiles)
		: lif::Component(owner)
	{
		_declComponent<Sounded>();
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
