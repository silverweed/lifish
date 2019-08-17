#pragma once

#include "Action.hpp"
#include "Screen.hpp"
#include "game.hpp"
#include <vector>

namespace lif {

class ShadedText;

namespace ui {

class PreferencesScreen : public lif::ui::Screen {
	enum class VolumeType { MUSIC, SOUND };
	enum class VolumeAction { RAISE, LOWER, MUTE_TOGGLE };

	constexpr static unsigned MAX_VOLUME = 15;
	constexpr static unsigned SPEAKER_SPRITE_SIZE = 25;

	std::vector<sf::VideoMode> fullscreenModes;

	int desiredFullscreenModeIdx = 0;
	bool desiredFullscreen;

	int desiredNPlayers;

	/** Hook for the music volume bar (used in draw()) */
	lif::ShadedText *musicVolumeBar = nullptr;
	/** Hook for the sounds volume bar (used in draw()) */
	lif::ShadedText *soundsVolumeBar = nullptr;

	short relMusicVolume = MAX_VOLUME,
	      relSoundVolume = MAX_VOLUME;

	/** Used to toggle music muteness */
	float prevMusicVolume = -1;
	sf::Texture *speakerTexture = nullptr;

	lif::ui::Action _changeVolume(VolumeType which, VolumeAction what);
	lif::ui::Action _changeNPlayers(int newNPlayer);
	lif::ui::Action _muteToggle(VolumeType which);
	void _setVolumeBar(VolumeType which);
	/** Adjust the initial values of preferences based on what we may have loaded from persistent preferences. */
	void _adjustPreferences();
	void _setupCallbacks();
	void _setupTransitions();

	const char* _getFullscreenText() const {
		return desiredFullscreen ? "YES" : "NO";
	}

	std::string _getFullscreenResText() const {
		const auto& vm = fullscreenModes[desiredFullscreenModeIdx];
		std::stringstream ss;
		ss << vm.width << "x" << vm.height << ":" << vm.bitsPerPixel;
		return desiredFullscreen ? ss.str() : "-";
	}

public:
	static constexpr const char *SCREEN_NAME = "preferences";

	explicit PreferencesScreen(const sf::RenderWindow& window, const sf::Vector2u& size);

	void onLoad() override;
	void onUnload() override;
};

}

}
