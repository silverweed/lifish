#pragma once

#include <memory>

class LoopingMusic;

namespace lif {

class MusicManager final {
	std::weak_ptr<LoopingMusic> music;

public:
	MusicManager& set(std::shared_ptr<LoopingMusic> m);
	MusicManager& play();
	MusicManager& stop();
	MusicManager& pause();
	MusicManager& setVolume(float volume);
};

}
