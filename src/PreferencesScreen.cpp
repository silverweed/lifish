#include "PreferencesScreen.hpp"
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
	texts["music_volume_down"] = text;

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
	texts["music_volume_up"] = text;

	// FX Volume
	text = new Game::ShadedText(font, "FX:", sf::Vector2f(ipadx, ipady + bounds.height + 20));
	text->setCharacterSize(size);
	elements.push_back(text);

	pos = text->getPosition();
	text = new Game::ShadedText(font, "-", sf::Vector2f(ipadx + 150, pos.y - 8));
	text->setCharacterSize(32);
	texts["sounds_volume_down"] = text;

	text = new Game::ShadedText(font, ss.str(), sf::Vector2f(ipadx + 200, pos.y));
	text->setCharacterSize(20);
	soundsVolumeBar = text;
	elements.push_back(text);

	bounds = text->getGlobalBounds();
	text = new Game::ShadedText(font, "+", sf::Vector2f(ipadx + 200 + bounds.width + 40, pos.y));
	text->setCharacterSize(30);
	texts["sounds_volume_up"] = text;

	text = new Game::ShadedText(font, "Controls", sf::Vector2f(ipadx, pos.y + bounds.height + 20));
	text->setCharacterSize(size);
	texts["controls"] = text;
	
	text = new Game::ShadedText(font, "Exit", pos);
	text->setCharacterSize(size);
	text->setPosition(Game::center(text->getGlobalBounds(), win_bounds));
	texts["exit"] = text;
}

void PreferencesScreen::changeMusicVolume(bool raise) {
	_changeVolume(raise, true);
}

void PreferencesScreen::changeSoundsVolume(bool raise) {
	_changeVolume(raise, false);
}

void PreferencesScreen::_changeVolume(bool raise, bool music) {
	short &vol = music ? relMusicVolume : relSoundVolume; 

	if ((raise && vol == MAX_VOLUME) || (!raise && vol == 0))
		return;

	vol += (raise ? 1 : -1);

	std::stringstream ss;
	for (unsigned short i = 0; i < vol; ++i) {
		ss << "|";	
	}

	if (music) {
		Game::music_volume = vol * 100 / MAX_VOLUME;
		musicVolumeBar->setString(ss.str());
	} else {
		Game::sounds_volume = vol * 100 / MAX_VOLUME;
		soundsVolumeBar->setString(ss.str());
	}
}
