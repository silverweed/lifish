#pragma once

#include "LoopingMusic.hpp"

namespace Game {

class MusicManager final {
	LoopingMusic *music = nullptr;

public:
	MusicManager& set(LoopingMusic *m);
	MusicManager& play();
	MusicManager& stop();
	MusicManager& setVolume(float volume);
};

}
