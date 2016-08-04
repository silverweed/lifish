#pragma once

#include "LoopingMusic.hpp"

namespace Game {

class MusicManager final {
	LoopingMusic *music = nullptr;

public:
	void set(LoopingMusic *m) { music = m; }
	void play();
	void stop();
	void setVolume(float volume);
};

}
