#pragma once

#include <array>
#include "Game.hpp"

namespace Game {

class Options {
public:
	/** The music volume */
	float music_volume;

	/** The FX volume */
	float sounds_volume;
	bool sounds_mute;

	/** -1 if i-th player doesn't use joystick, else the joystick index. */
	std::array<short, Game::MAX_PLAYERS> useJoystick;
};

}
