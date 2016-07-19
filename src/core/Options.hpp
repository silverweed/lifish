#pragma once

#include <array>
#include "game.hpp"

namespace Game {

struct Options {
	/** The music volume */
	float musicVolume;

	/** The FX volume */
	float soundsVolume;
	bool soundsMute;

	bool showFPS = false;
};

}
