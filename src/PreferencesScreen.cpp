#include "PreferencesScreen.hpp"
#include "LoopingMusic.hpp"
#include "Options.hpp"
#include "utils.hpp"
#include <sstream>

using Game::PreferencesScreen;

PreferencesScreen::PreferencesScreen() : Screen() {
	/* Layout:
	 * 
	 * Music volume
	 * FX volume
	 * Controls
	 * (planned): game mode (BOOM-compatible, extended)
	 * (planned): game difficulty
	 * Exit
	 */
	const auto font = Game::getAsset("fonts", Game::Fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);

	const auto size = 24;
	const float ipadx = 25,
		    ipady = 15;
	auto text = new Game::ShadedText(font, "Music:", sf::Vector2f(ipadx, ipady));
	text->setCharacterSize(size);
	elements.push_back(text);

	// Music volume
	auto pos = text->getPosition();

	text = new Game::ShadedText(font, "-", sf::Vector2f(ipadx + 150, ipady - 8));
	text->setCharacterSize(32);
	texts["preferences::music_volume_down"] = text;

	text = new Game::ShadedText(font, "placeholder", sf::Vector2f(ipadx + 200, ipady));
	// Draw the full volume bar to get the measure of this element's max width
	// (also, the volume is maxed by default, so we don't need to do any further checks here)
	std::stringstream ss;
	for (unsigned short i = 0; i < MAX_VOLUME; ++i) {
		ss << "|";	
	}
	text->setCharacterSize(20);
	text->setString(ss.str());
	musicVolumeBar = text;
	elements.push_back(text);

	auto bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font, "+", sf::Vector2f(ipadx + 200 + bounds.width + 40, ipady - 6));
	text->setCharacterSize(30);
	texts["preferences::music_volume_up"] = text;

	bounds = text->getGlobalBounds();
	auto image = new sf::Sprite;
	speakerTexture = Game::cache.loadTexture(Game::getAsset("graphics", "speaker.png"));
	image->setTexture(speakerTexture);
	image->setTextureRect(sf::IntRect(prevMusicVolume >= 0 ? 25 : 0, 0, 25, 25));
	image->setPosition(sf::Vector2f(bounds.left + bounds.width + 20, ipady));
	images["preferences::music_mute_toggle"] = image;

	// FX Volume
	text = new Game::ShadedText(font, "FX:", sf::Vector2f(ipadx, ipady + bounds.height + 20));
	text->setCharacterSize(size);
	elements.push_back(text);

	pos = text->getPosition();
	text = new Game::ShadedText(font, "-", sf::Vector2f(ipadx + 150, pos.y - 8));
	text->setCharacterSize(32);
	texts["preferences::sounds_volume_down"] = text;

	text = new Game::ShadedText(font, ss.str(), sf::Vector2f(ipadx + 200, pos.y));
	text->setCharacterSize(20);
	soundsVolumeBar = text;
	elements.push_back(text);

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font, "+", sf::Vector2f(ipadx + 200 + bounds.width + 40, pos.y - 6));
	text->setCharacterSize(30);
	texts["preferences::sounds_volume_up"] = text;

	bounds = text->getGlobalBounds();
	image = new sf::Sprite;
	speakerTexture = Game::cache.loadTexture(Game::getAsset("graphics", "speaker.png"));
	image->setTexture(speakerTexture);
	image->setTextureRect(sf::IntRect(Game::options.soundsMute ? SPEAKER_SPRITE_SIZE : 0,
				0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
	image->setPosition(sf::Vector2f(bounds.left + bounds.width + 20, bounds.top));
	images["preferences::sounds_mute_toggle"] = image;

	text = new Game::ShadedText(font, "Controls", sf::Vector2f(ipadx, pos.y + bounds.height + 20));
	text->setCharacterSize(size);
	texts["preferences::controls"] = text;
	
	text = new Game::ShadedText(font, "Exit", pos);
	text->setCharacterSize(size);
	bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(Game::center(bounds, win_bounds).x, win_bounds.height - 2 * bounds.height));
	texts["exit"] = text;
}

void PreferencesScreen::changeVolume(VolumeType which, VolumeAction what) {
	if (what == VolumeAction::MUTE_TOGGLE) {
		switch (which) {
		case VolumeType::MUSIC: 
			if (prevMusicVolume < 0) {
				// unmute->mute
				prevMusicVolume = Game::options.musicVolume;
				Game::options.musicVolume = 0;
			} else {
				//mute->unmute
				Game::options.musicVolume = prevMusicVolume;
				prevMusicVolume = -1;
			}
			images["preferences::music_mute_toggle"]->setTextureRect(
					sf::IntRect(prevMusicVolume >= 0 ? SPEAKER_SPRITE_SIZE : 0,
						0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
			if (Game::music != nullptr)
				Game::music->setVolume(Game::options.musicVolume);
			return;
		case VolumeType::SOUND:
			Game::options.soundsMute = !Game::options.soundsMute;
			images["preferences::sounds_mute_toggle"]->setTextureRect(
					sf::IntRect(Game::options.soundsMute ? SPEAKER_SPRITE_SIZE : 0,
						0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
			return;
		default: 
			return;
		}
	}
	short &vol = which == VolumeType::MUSIC ? relMusicVolume : relSoundVolume; 
	const bool raise = what == VolumeAction::RAISE;

	if ((raise && vol == MAX_VOLUME) || (!raise && vol == 0))
		return;

	vol += (raise ? 1 : -1);

	std::stringstream ss;
	for (unsigned short i = 0; i < vol; ++i) {
		ss << "|";	
	}

	if (which == VolumeType::MUSIC) {
		Game::options.musicVolume = vol * 100 / MAX_VOLUME;
		musicVolumeBar->setString(ss.str());
		if (Game::music != nullptr)
			Game::music->setVolume(Game::options.musicVolume);
	} else {
		Game::options.soundsVolume = vol * 100 / MAX_VOLUME;
		soundsVolumeBar->setString(ss.str());
	}
}
