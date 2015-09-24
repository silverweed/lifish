#pragma once

#include "HomeScreen.hpp"
#include "PauseScreen.hpp"

namespace Game {

/**
 * Singleton class for the Preferences screen
 */
class PreferencesScreen : public Game::Screen {
	constexpr static unsigned short MAX_VOLUME = 15;
	constexpr static unsigned short SPEAKER_SPRITE_SIZE = 25;

	/** Hook for the music volume bar (used in draw()) */
	Game::ShadedText *musicVolumeBar = nullptr;
	/** Hook for the sounds volume bar (used in draw()) */
	Game::ShadedText *soundsVolumeBar = nullptr;

	short relMusicVolume = MAX_VOLUME,
	      relSoundVolume = MAX_VOLUME;

	/** Used to toggle music muteness */
	float prevMusicVolume = -1;

	sf::Texture speakerTexture;


	PreferencesScreen();
public:
	enum class VolumeType { MUSIC, SOUND };
	enum class VolumeAction { RAISE, LOWER, MUTE_TOGGLE };

	static PreferencesScreen& getInstance() {
		static PreferencesScreen instance;
		return instance;
	}

	void changeVolume(VolumeType which, VolumeAction what);

	std::set<Game::Screen*> getParents() override {
		return {
			&Game::HomeScreen::getInstance(),
			&Game::PauseScreen::getInstance()
		};
	}
};

}
