#include "PreferencesScreen.hpp"
#include "GameCache.hpp"
#include "Interactable.hpp"
#include "MusicManager.hpp"
#include "Options.hpp"
#include "ShadedText.hpp"
#include "game.hpp"
#include "preferences_persistence.hpp"
#include "utils.hpp"
#include <iostream>
#include <memory>

using lif::ui::PreferencesScreen;
using lif::ui::Interactable;
using Action = lif::ui::Action;

PreferencesScreen::PreferencesScreen(const sf::RenderWindow& window, const sf::Vector2u& sz)
	: lif::ui::Screen(window, sz)
{
	name = SCREEN_NAME;
	parent = "home";
	_loadBGSprite(lif::getAsset("graphics", "screenbg1.png"));

	// Gather fullscreen modes
	fullscreenModes = sf::VideoMode::getFullscreenModes();

	/* Layout:
	 *
	 * MUSIC: - ||||||| + (m)
	 * FX:    - ||||||| + (m)
	 * Fullscreen: yes/no
	 * Resolution: res
	 * Controls
	 *
	 * OK   Back 
	 */
	const auto font = lif::getAsset("fonts", lif::fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, sz.x, sz.y);
	const auto size = 24;
	const float ipadx = 25,
		    ipady = 15;

	auto text = new lif::ShadedText(font, "Music:", sf::Vector2f(ipadx, ipady));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.emplace_back(text);

	// Music volume
	auto pos = text->getPosition();

	const auto upperTextAlign1 = ipadx + 200;

	text = new lif::ShadedText(font, "-", sf::Vector2f(upperTextAlign1, ipady - 8));
	text->setCharacterSize(34);
	interactables["music_volume_down"] = std::make_unique<Interactable>(text);

	text = new lif::ShadedText(font, "placeholder", sf::Vector2f(upperTextAlign1 + 50, ipady));
	// Draw the full volume bar to get the measure of this element's max width
	// (also, the volume is maxed by default, so we don't need to do any further checks here)
	std::stringstream ss;
	for (unsigned i = 0; i < MAX_VOLUME; ++i) {
		ss << "|";
	}
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(20);
	text->setString(ss.str());
	musicVolumeBar = text;
	nonInteractables.emplace_back(text);

	auto bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, "+", sf::Vector2f(upperTextAlign1 + 50 + bounds.width + 40, ipady - 6));
	text->setCharacterSize(30);
	interactables["music_volume_up"] = std::make_unique<Interactable>(text);

	bounds = text->getGlobalBounds();
	auto image = new sf::Sprite;
	speakerTexture = lif::cache.loadTexture(lif::getAsset("graphics", "speaker.png"));
	image->setTexture(*speakerTexture);
	image->setTextureRect(sf::IntRect(prevMusicVolume >= 0 ? 25 : 0, 0, 25, 25));
	image->setPosition(sf::Vector2f(bounds.left + bounds.width + 20, ipady));
	interactables["music_mute_toggle"] = std::make_unique<Interactable>(image);

	// FX Volume
	text = new lif::ShadedText(font, "FX:", sf::Vector2f(ipadx, ipady + bounds.height + 20));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.emplace_back(text);

	pos = text->getPosition();
	text = new lif::ShadedText(font, "-", sf::Vector2f(upperTextAlign1, pos.y - 8));
	text->setCharacterSize(34);
	interactables["sounds_volume_down"] = std::make_unique<Interactable>(text);

	text = new lif::ShadedText(font, ss.str(), sf::Vector2f(upperTextAlign1 + 50, pos.y));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(20);
	soundsVolumeBar = text;
	nonInteractables.emplace_back(text);

	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, "+", sf::Vector2f(upperTextAlign1 + 50 + bounds.width + 40, pos.y - 6));
	text->setCharacterSize(30);
	interactables["sounds_volume_up"] = std::make_unique<Interactable>(text);

	bounds = text->getGlobalBounds();
	image = new sf::Sprite;
	speakerTexture = lif::cache.loadTexture(lif::getAsset("graphics", "speaker.png"));
	image->setTexture(*speakerTexture);
	image->setTextureRect(sf::IntRect(lif::options.soundsMute ? SPEAKER_SPRITE_SIZE : 0,
				0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
	image->setPosition(sf::Vector2f(bounds.left + bounds.width + 20, bounds.top));
	interactables["sounds_mute_toggle"] = std::make_unique<Interactable>(image);

	// # players
	text = new lif::ShadedText(font, "Players: ", sf::Vector2f(ipadx, pos.y + bounds.height + 20));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.emplace_back(text);

	pos = text->getPosition();
	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, lif::to_string(lif::options.nPlayers), sf::Vector2f(upperTextAlign1, pos.y));
	text->setCharacterSize(size);
	interactables["n_players"] = std::make_unique<Interactable>(text);

	// Fullscreen
	text = new lif::ShadedText(font, "Fullscreen", sf::Vector2f(ipadx, pos.y + bounds.height + 20));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.emplace_back(text);

	pos = text->getPosition();
	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, _getFullscreenText(), sf::Vector2f(upperTextAlign1, pos.y));
	text->setCharacterSize(size);
	interactables["fullscreen"] = std::make_unique<Interactable>(text);

	// Fullscreen resolution
	pos = text->getPosition();
	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, "Fllscr.Res.", sf::Vector2f(ipadx, pos.y + bounds.height + 20));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.emplace_back(text);

	pos = text->getPosition();
	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, _getFullscreenResText(), sf::Vector2f(upperTextAlign1, pos.y));
	text->setCharacterSize(size);
	interactables["fullscreen_res"] = std::make_unique<Interactable>(text);

	// Controls
	text = new lif::ShadedText(font, "Controls", sf::Vector2f(ipadx, pos.y + bounds.height + 20));
	text->setCharacterSize(size);
	interactables["controls"] = std::make_unique<Interactable>(text);

	// Confirm
	text = new lif::ShadedText(font, "OK", pos);
	text->setCharacterSize(size);
	bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, win_bounds).x - 100,
				win_bounds.height - 3 * bounds.height));
	interactables["confirm"] = std::make_unique<Interactable>(text);

	// Back
	text = new lif::ShadedText(font, "Cancel", pos);
	text->setCharacterSize(size);
	bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, win_bounds).x + 100, win_bounds.height - 3 * bounds.height));
	interactables["back"] = std::make_unique<Interactable>(text);

	_adjustPreferences();
	_setupCallbacks();
	_setupTransitions();
}

void PreferencesScreen::onLoad() {
	lif::ui::Screen::onLoad();
	desiredFullscreen = lif::options.fullscreen;
	
	if (desiredFullscreen) {
		const auto& vm = lif::options.videoMode;
		for (unsigned i = 0; i < fullscreenModes.size(); ++i) {
			if (vm == fullscreenModes[i]) {
				desiredFullscreenModeIdx = i;
				break;
			}
		}
	}

	interactables["fullscreen"]->getText()->setString(_getFullscreenText());
	interactables["fullscreen_res"]->getText()->setString(_getFullscreenResText());
}

void PreferencesScreen::_adjustPreferences() {
	relMusicVolume = static_cast<short>(MAX_VOLUME * lif::options.musicVolume / 100.f);
	relSoundVolume = static_cast<short>(MAX_VOLUME * lif::options.soundsVolume / 100.f);
	_setVolumeBar(VolumeType::MUSIC);
	_setVolumeBar(VolumeType::SOUND);
	if (lif::options.soundsMute)
		interactables["sounds_mute_toggle"]->getSprite()->setTextureRect(
				sf::IntRect(lif::options.soundsMute ? SPEAKER_SPRITE_SIZE : 0,
					0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
}

void PreferencesScreen::_setupCallbacks() {
	callbacks["music_volume_up"] = [this] () {
		return _changeVolume(VolumeType::MUSIC, VolumeAction::RAISE);
	};
	callbacks["music_volume_down"] = [this] () {
		return _changeVolume(VolumeType::MUSIC, VolumeAction::LOWER);
	};
	callbacks["music_mute_toggle"] = [this] () {
		return _muteToggle(VolumeType::MUSIC);
	};
	callbacks["sounds_volume_up"] = [this] () {
		return _changeVolume(VolumeType::SOUND, VolumeAction::RAISE);
	};
	callbacks["sounds_volume_down"] = [this] () {
		return _changeVolume(VolumeType::SOUND, VolumeAction::LOWER);
	};
	callbacks["sounds_mute_toggle"] = [this] () {
		return _muteToggle(VolumeType::SOUND);
	};
	callbacks["n_players"] = [this] () {
		auto n = lif::options.nPlayers + 1;
		if (n > lif::MAX_PLAYERS) n -= lif::MAX_PLAYERS;
		return _changeNPlayers(n);
	};
	callbacks["fullscreen"] = [this] () {
		desiredFullscreen = !desiredFullscreen;
		interactables["fullscreen"]->getText()->setString(_getFullscreenText());
		interactables["fullscreen_res"]->getText()->setString(_getFullscreenResText());
		return Action::DO_NOTHING;
	};
	callbacks["fullscreen_res"] = [this] () {
		if (desiredFullscreen) {
			desiredFullscreenModeIdx = (desiredFullscreenModeIdx + 1) % fullscreenModes.size();
			interactables["fullscreen_res"]->getText()->setString(_getFullscreenResText());
		}
		return Action::DO_NOTHING;
	};
	callbacks["confirm"] = [this] () {
		lif::options.fullscreen = desiredFullscreen;
		lif::options.videoMode = fullscreenModes[desiredFullscreenModeIdx];
		return Action::SWITCH_TO_PARENT;
	};
}

void PreferencesScreen::_setupTransitions() {
	using D = lif::Direction;
	transitions.add("music_volume_down",  std::make_pair(D::RIGHT, "music_volume_up"));
	transitions.add("music_volume_down",  std::make_pair(D::DOWN,  "sounds_volume_down"));
	transitions.add("music_volume_down",  std::make_pair(D::LEFT,  "music_mute_toggle"));
	transitions.add("music_volume_up",    std::make_pair(D::RIGHT, "music_mute_toggle"));
	transitions.add("sounds_volume_down", std::make_pair(D::RIGHT, "sounds_volume_up"));
	transitions.add("sounds_volume_down", std::make_pair(D::LEFT,  "sounds_mute_toggle"));
	transitions.add("sounds_volume_down", std::make_pair(D::DOWN,  "n_players"));
	transitions.add("sounds_volume_up",   std::make_pair(D::RIGHT, "sounds_mute_toggle"));
	transitions.add("sounds_volume_up",   std::make_pair(D::DOWN,  "n_players"));
	transitions.add("sounds_mute_toggle", std::make_pair(D::DOWN,  "n_players"));
	transitions.add("music_volume_up",    std::make_pair(D::DOWN,  "sounds_volume_up"));
	transitions.add("music_mute_toggle",  std::make_pair(D::DOWN,  "sounds_mute_toggle"));
	transitions.add("n_players",          std::make_pair(D::DOWN,  "controls"));
	transitions.add("controls",           std::make_pair(D::DOWN,  "back"));
	transitions.add("back",               std::make_pair(D::DOWN,  "music_volume_down"));
}

Action PreferencesScreen::_changeVolume(VolumeType which, VolumeAction what) {
	short& vol = which == VolumeType::MUSIC ? relMusicVolume : relSoundVolume;
	const bool raise = what == VolumeAction::RAISE;

	if ((raise && vol == MAX_VOLUME) || (!raise && vol == 0))
		return Action::DO_NOTHING;

	vol += (raise ? 1 : -1);
	_setVolumeBar(which);

	return Action::DO_NOTHING;
}

Action PreferencesScreen::_muteToggle(VolumeType which) {
	// TODO better mute handling
	// Why do we handle music mute differently from sound?
	switch (which) {
	case VolumeType::MUSIC:
		if (prevMusicVolume < 0) {
			// unmute->mute
			prevMusicVolume = lif::options.musicVolume;
			lif::options.musicVolume = 0;
		} else {
			// mute->unmute
			lif::options.musicVolume = prevMusicVolume;
			prevMusicVolume = -1;
		}
		interactables["music_mute_toggle"]->getSprite()->setTextureRect(
				sf::IntRect(prevMusicVolume >= 0 ? SPEAKER_SPRITE_SIZE : 0,
					0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
		if (lif::musicManager != nullptr)
			lif::musicManager->setVolume(lif::options.musicVolume);
		break;
	case VolumeType::SOUND:
		lif::options.soundsMute = !lif::options.soundsMute;
		interactables["sounds_mute_toggle"]->getSprite()->setTextureRect(
				sf::IntRect(lif::options.soundsMute ? SPEAKER_SPRITE_SIZE : 0,
					0, SPEAKER_SPRITE_SIZE, SPEAKER_SPRITE_SIZE));
		break;
	default:
		break;
	}

	return Action::DO_NOTHING;
}

void PreferencesScreen::_setVolumeBar(VolumeType which) {
	const auto vol = which == VolumeType::MUSIC ? relMusicVolume : relSoundVolume;
	std::stringstream ss;
	for (int i = 0; i < vol; ++i) {
		ss << "|";
	}

	if (which == VolumeType::MUSIC) {
		lif::options.musicVolume = vol * 100 / MAX_VOLUME;
		musicVolumeBar->setString(ss.str());
		if (lif::musicManager != nullptr)
			lif::musicManager->setVolume(lif::options.musicVolume);
	} else {
		lif::options.soundsVolume = vol * 100 / MAX_VOLUME;
		soundsVolumeBar->setString(ss.str());
	}
}

lif::ui::Action PreferencesScreen::_changeNPlayers(int newNPlayer) {
	assert(newNPlayer > 0 && newNPlayer <= lif::MAX_PLAYERS);
	lif::options.nPlayers = newNPlayer;
	interactables["n_players"]->getText()->setString(lif::to_string(newNPlayer));

	return Action::DO_NOTHING;
}

void PreferencesScreen::onUnload() {
	lif::ui::Screen::onUnload();
	lif::savePreferences(lif::PREFERENCES_SAVE_FILE_NAME);
}
