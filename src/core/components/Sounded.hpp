#pragma once

#include <string>
#include <unordered_map>
#include "Component.hpp"
#include "sid.hpp"

/**
 * An object which has one or more sounds associated with it.
 */
namespace lif {

class Sounded : public lif::Component {
	// soundName => soundFile
	std::unordered_map<lif::StringId, std::string> soundFiles;

	template <typename...Args>
	void _addSounds(lif::StringId k, const std::string& v, Args... args) {
		static_assert(sizeof...(args) % 2 == 0, "addSounds() must be given an even number of args!");
		soundFiles[k] = v;
		_addSounds(std::forward<Args>(args)...);
	}

	void _addSounds() {}
public:

	template <typename...Args>
	explicit Sounded(lif::Entity& owner, Args... _soundFiles)
		: lif::Component(owner)
	{
		_declComponent<Sounded>();
		_addSounds(std::forward<Args>(_soundFiles)...);
	}

	std::string getSoundFile(lif::StringId name) const {
		auto it = soundFiles.find(name);
		if (it == soundFiles.end()) return "";
		return it->second;
	}

	std::string getSoundFile(const char *name) const {
		return getSoundFile(lif::sid(name));
	}

	void setSoundFile(lif::StringId name, const std::string& file) {
		soundFiles[name] = file;
	}

	void setSoundFile(const char *name, const std::string& file) {
		setSoundFile(lif::sid(name), file);
	}
};

}
