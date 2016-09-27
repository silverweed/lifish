#pragma once

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
