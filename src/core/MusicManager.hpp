#pragma once

#include <memory>
#include <SFML/System/NonCopyable.hpp>

class LoopingMusic;

namespace lif {

class MusicManager final : private sf::NonCopyable {
	std::weak_ptr<LoopingMusic> music;

public:
	MusicManager& set(std::shared_ptr<LoopingMusic> m);
	MusicManager& play();
	MusicManager& stop();
	MusicManager& pause();
	MusicManager& setVolume(float volume);
};

}
