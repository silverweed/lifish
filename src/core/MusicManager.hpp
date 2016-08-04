#pragma once

#include "LoopingMusic.hpp"

namespace Game {

class MusicManager final {
	LoopingMusic *music = nullptr;

public:
	LoopingMusic* set(LoopingMusic *m) { return music = m; }
	void play();
	void stop();
	void setVolume(float volume);
};

}
