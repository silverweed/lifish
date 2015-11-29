#pragma once

#include <array>
#include "Game.hpp"

namespace Game {

class Options {
public:
	/** The music volume */
	float musicVolume;

	/** The FX volume */
	float soundsVolume;
	bool soundsMute;

	/** -1 if i-th player doesn't use joystick, else the joystick index. */
	std::array<short, Game::MAX_PLAYERS> useJoystick;

	bool showFPS = false;
};

}
