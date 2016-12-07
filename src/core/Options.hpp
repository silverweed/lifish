#pragma once

namespace Game {

struct Options {
	/** The music volume */
	float musicVolume;

	/** The FX volume */
	float soundsVolume;
	bool soundsMute;

	bool showFPS;
	/** If true, print to console time stats for the drawing phase */
	bool printDrawStats;

	bool vsync;
	unsigned short framerateLimit;

	/** This is the designed size of the application. Used, for example, to correctly
	 *  resize the content when window is resized. Must be set manually.
	 */
	sf::Vector2u windowSize;
};

}
