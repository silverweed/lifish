#pragma once

#include "Screen.hpp"

namespace Game {

/**
 * Singleton class for the Preferences screen
 */
class PreferencesScreen : public Game::Screen {
	constexpr static unsigned short MAX_VOLUME = 15;

	/** Hook for the music volume bar (used in draw()) */
	Game::ShadedText *musicVolumeBar = nullptr;
	/** Hook for the sounds volume bar (used in draw()) */
	Game::ShadedText *soundsVolumeBar = nullptr;

	short relMusicVolume = MAX_VOLUME,
	      relSoundVolume = MAX_VOLUME;


	PreferencesScreen();

	/** If music == true, changes the music volume, else the FX volume. */
	void _changeVolume(bool raise, bool music);
public:
	static PreferencesScreen& getInstance() {
		static PreferencesScreen instance;
		return instance;
	}

	void changeMusicVolume(bool raise);
	void changeSoundsVolume(bool raise);
};

}
