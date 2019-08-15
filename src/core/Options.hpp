#pragma once

namespace lif {

struct Options {
	/** The music volume */
	float musicVolume = 0;

	/** The FX volume */
	float soundsVolume = 0;
	bool soundsMute = false;

	bool vsync = true;
	int framerateLimit = 60;

	/** This is the designed size of the application. Used, for example, to correctly
	 *  resize the content when window is resized. Must be set manually.
	 */
	sf::Vector2u windowSize;

	/** How many players are spawned, from 1 to MAX_PLAYERS */
	int nPlayers = 1;

#ifndef RELEASE
	bool showFPS = false;
	/** If true, print to console time stats for the drawing phase */
	bool printDrawStats = false;
#endif

};

}
