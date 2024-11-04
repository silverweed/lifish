#include "PreferencesScreen.hpp"
#include "GameCache.hpp"
#include "Interactable.hpp"
#include "MusicManager.hpp"
#include "Options.hpp"
#include "ShadedText.hpp"
#include "Time.hpp"
#include "game.hpp"
#include "preferences_persistence.hpp"
#include "utils.hpp"
#include "language.hpp"
#include <iostream>
#include <memory>

using lif::ui::PreferencesScreen;
using lif::ui::Interactable;
using Action = lif::ui::Action;

static constexpr std::array<int, 5> FRAMERATE_LIMITS = { 60, 120, 144, 240, 0 };
static unsigned curFramerateIdx;

PreferencesScreen::PreferencesScreen(const sf::RenderWindow& window, const sf::Vector2u& sz)
	: lif::ui::DynamicScreen(window, sz)
{
	build();
}

void PreferencesScreen::build() {
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
	 * Show FPS: yes/no
	 * VSync: yes/no
	 * FramerateLimit: num
	 * GameTimer: yes/no
	 * Controls
	 *
	 * OK   Back
	 */
	const auto font = lif::getAsset("fonts", lif::fonts::SCREEN);
	const auto winBounds = sf::FloatRect(0, 0, size.x, size.y);
	const auto size = 20;
	const float ipadx = 25,
		    ipady = 15;
	const float rowSize = 32;

	auto text = new lif::ShadedText(font, lif::getLocalized("music") + ":", sf::Vector2f(ipadx, ipady));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.emplace_back(text);

	int nRow = 0;

	// Music volume
	auto pos = text->getPosition();
	const auto colAlign = ipadx + 220;

	text = new lif::ShadedText(font, "-", sf::Vector2f(colAlign, ipady - 8));
	text->setCharacterSize(34);
	interactables["music_volume_down"] = std::make_unique<Interactable>(text);

	text = new lif::ShadedText(font, "placeholder", sf::Vector2f(colAlign + 50, ipady + nRow * rowSize));
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
	text = new lif::ShadedText(font, "+", sf::Vector2f(colAlign + 50 + bounds.width + 40, ipady - 6));
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
	++nRow;
	text = new lif::ShadedText(font, lif::getLocalized("fx") + ":", sf::Vector2f(ipadx, ipady + nRow * rowSize));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.emplace_back(text);

	pos = text->getPosition();
	text = new lif::ShadedText(font, "-", sf::Vector2f(colAlign, pos.y - 8));
	text->setCharacterSize(34);
	interactables["sounds_volume_down"] = std::make_unique<Interactable>(text);

	text = new lif::ShadedText(font, ss.str(), sf::Vector2f(colAlign + 50, pos.y));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(20);
	soundsVolumeBar = text;
	nonInteractables.emplace_back(text);

	bounds = text->getGlobalBounds();
	text = new lif::ShadedText(font, "+", sf::Vector2f(colAlign + 50 + bounds.width + 40, pos.y - 6));
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

#define SIMPLE_ROW(_header, _name, _content) \
	++nRow; \
	text = new lif::ShadedText(font, _header, sf::Vector2f(ipadx, ipady + nRow * rowSize)); \
	text->setShadowSpacing(2, 2); \
	text->setCharacterSize(size); \
	nonInteractables.emplace_back(text); \
	\
	pos = text->getPosition(); \
	text = new lif::ShadedText(font, _content, sf::Vector2f(colAlign, pos.y)); \
	text->setCharacterSize(size); \
	interactables[_name] = std::make_unique<Interactable>(text)

	SIMPLE_ROW(lif::getLocalized("players") + ": ", "n_players", lif::to_string(lif::options.nPlayers));

	if (fullscreenModes.size() > 0) {
		SIMPLE_ROW(lif::getLocalized("fullscreen"), "fullscreen", _getFullscreenText());
		SIMPLE_ROW(lif::getLocalized("fullscreen_res"), "fullscreen_res", _getFullscreenResText());
	}
	SIMPLE_ROW(lif::getLocalized("show_fps"), "show_fps", "");
	SIMPLE_ROW("Vsync", "vsync", "");
	SIMPLE_ROW(lif::getLocalized("framerate_limit"), "framerate_limit", lif::to_string(lif::options.framerateLimit));
	SIMPLE_ROW(lif::getLocalized("show_game_timer"), "show_game_timer", "");

	// Controls
	++nRow;
	text = new lif::ShadedText(font, lif::getLocalized("controls"), sf::Vector2f(ipadx, 2 * ipady + nRow * rowSize));
	text->setCharacterSize(size);
	interactables["controls"] = std::make_unique<Interactable>(text);

	// OK
	text = new lif::ShadedText(font, "OK", pos);
	text->setCharacterSize(size);
	bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, winBounds).x - 100, winBounds.height - 3 * bounds.height));
	interactables["ok"] = std::make_unique<Interactable>(text);

	// Back
	text = new lif::ShadedText(font, lif::getLocalized("cancel"), pos);
	text->setCharacterSize(size);
	bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, winBounds).x + 100, winBounds.height - 3 * bounds.height));
	interactables["back"] = std::make_unique<Interactable>(text);

	// Confirm resolution
	confirmResText = new lif::ShadedText(font, lif::getLocalized("is_this_ok?"), pos);
	confirmResText->setCharacterSize(size);
	bounds = confirmResText->getGlobalBounds();
	confirmResText->setPosition(sf::Vector2f(lif::center(bounds, winBounds).x, 280));
	confirmResText->setShadowSpacing(2, 2);

	pos = confirmResText->getPosition();
	confirmResTimeText = new lif::ShadedText(font, "", pos);
	confirmResTimeText->setCharacterSize(size);
	confirmResTimeText->setPosition(pos + sf::Vector2f(bounds.width + 25, 0));
	confirmResTimeText->setShadowSpacing(2, 2);

	confirmResYes = new lif::ShadedText(font, lif::getLocalized("yes"), pos);
	confirmResYes->setCharacterSize(size);
	bounds = confirmResYes->getGlobalBounds();
	confirmResYes->setPosition(sf::Vector2f(
		lif::center(bounds, winBounds).x - bounds.width, pos.y + bounds.height + 15));

	pos = confirmResYes->getPosition();
	confirmResNo = new lif::ShadedText(font, lif::getLocalized("no"), pos);
	confirmResNo->setCharacterSize(size);
	confirmResNo->setPosition(sf::Vector2f(pos.x + bounds.width + 50, pos.y));

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

	if (fullscreenModes.size() > 0) {
		interactables["fullscreen"]->getText()->setString(_getFullscreenText());
		interactables["fullscreen_res"]->getText()->setString(_getFullscreenResText());
	}
	interactables["n_players"]->getText()->setString(lif::to_string(lif::options.nPlayers));
	interactables["show_fps"]->getText()->setString(lif::options.showFPS
			? lif::getLocalized("yes") : lif::getLocalized("no"));
	interactables["vsync"]->getText()->setString(lif::options.vsync
			? lif::getLocalized("yes") : lif::getLocalized("no"));
	interactables["show_game_timer"]->getText()->setString(lif::options.showGameTimer
			? lif::getLocalized("yes") : lif::getLocalized("no"));
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
	callbacks["show_fps"] = [this] () {
		lif::options.showFPS = !lif::options.showFPS;
		interactables["show_fps"]->getText()->setString(lif::options.showFPS
				? lif::getLocalized("yes") : lif::getLocalized("no"));
		return Action::DO_NOTHING;
	};
	callbacks["show_game_timer"] = [this] () {
		lif::options.showGameTimer = !lif::options.showGameTimer;
		interactables["show_game_timer"]->getText()->setString(lif::options.showGameTimer
				? lif::getLocalized("yes") : lif::getLocalized("no"));
		return Action::DO_NOTHING;
	};
	callbacks["vsync"] = [this] () {
		lif::options.vsync = !lif::options.vsync;
		interactables["vsync"]->getText()->setString(lif::options.vsync
				? lif::getLocalized("yes") : lif::getLocalized("no"));
		return Action::DO_NOTHING;
	};
	callbacks["framerate_limit"] = [this] () {
		curFramerateIdx = (curFramerateIdx + 1) % FRAMERATE_LIMITS.size();
		lif::options.framerateLimit = FRAMERATE_LIMITS[curFramerateIdx];
		const auto str =
			lif::options.framerateLimit > 0
				? lif::to_string(lif::options.framerateLimit) : "Unlimited";
		interactables["framerate_limit"]->getText()->setString(str);
		return Action::DO_NOTHING;
	};
	callbacks["ok"] = [this] () {
		// const bool wasFullscreen = lif::options.fullscreen;
		// const auto formerVideoMode = lif::options.videoMode;
		lif::options.fullscreen = desiredFullscreen;
		if (fullscreenModes.size() > 0) {
			lif::options.videoMode = fullscreenModes[desiredFullscreenModeIdx];
		}
		// if (lif::options.fullscreen != wasFullscreen || lif::options.videoMode != formerVideoMode) {
		// 	if (lif::options.fullscreen)
		// 		_setMustConfirmRes(true);
		// 	return Action::DO_NOTHING;
		// }

		return Action::SWITCH_TO_PARENT;
	};
	callbacks["confirm_res_yes"] = [this] () {
		_setMustConfirmRes(false);
		return Action::DO_NOTHING;
	};
	callbacks["confirm_res_no"] = [this] () {
		_setMustConfirmRes(false);
		lif::options.fullscreen = false;
		desiredFullscreen = false;
		interactables["fullscreen"]->getText()->setString(_getFullscreenText());
		interactables["fullscreen_res"]->getText()->setString(_getFullscreenResText());
		return Action::DO_NOTHING;
	};
}

void PreferencesScreen::_setMustConfirmRes(bool must) {
	mustConfirmRes = must;
	if (must) {
		confirmResCountdown = sf::seconds(5);
		confirmResTimeText->setString(lif::to_string(static_cast<int>(confirmResCountdown.asSeconds())));
		nonInteractables.emplace_back(confirmResText);
		nonInteractables.emplace_back(confirmResTimeText);
		interactables["confirm_res_yes"] = std::make_unique<Interactable>(confirmResYes);
		interactables["confirm_res_no"] = std::make_unique<Interactable>(confirmResNo);
	} else {
		const auto l = nonInteractables.size();
		assert(l > 2);
		nonInteractables[l - 1].release();
		nonInteractables[l - 2].release();
		nonInteractables.pop_back();
		nonInteractables.pop_back();
		auto it = interactables.find("confirm_res_yes");
		assert(it != interactables.end());
		it->second.release();
		interactables.erase(it);
		it = interactables.find("confirm_res_no");
		assert(it != interactables.end());
		it->second.release();
		interactables.erase(it);
	}
}

void PreferencesScreen::update() {
	lif::ui::Screen::update();

	if (!mustConfirmRes)
		return;


	confirmResCountdown -= lif::time.getDelta();
	if (confirmResCountdown <= sf::Time::Zero) {
		_setMustConfirmRes(false);
		lif::options.fullscreen = false;
		desiredFullscreen = false;
		interactables["fullscreen"]->getText()->setString(_getFullscreenText());
		interactables["fullscreen_res"]->getText()->setString(_getFullscreenResText());
	} else {
		confirmResTimeText->setString(lif::to_string(static_cast<int>(confirmResCountdown.asSeconds())));
	}
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
	transitions.add("n_players",          std::make_pair(D::DOWN,  "fullscreen"));
	transitions.add("fullscreen",         std::make_pair(D::DOWN,  "fullscreen_res"));
	transitions.add("fullscreen_res",     std::make_pair(D::DOWN,  "show_fps"));
	transitions.add("show_fps",           std::make_pair(D::DOWN,  "vsync"));
	transitions.add("vsync",              std::make_pair(D::DOWN,  "framerate_limit"));
	transitions.add("framerate_limit",    std::make_pair(D::DOWN,  "show_game_timer"));
	transitions.add("show_game_timer",    std::make_pair(D::DOWN,  "controls"));
	transitions.add("controls",           std::make_pair(D::DOWN,  "ok"));
	transitions.add("ok",                 std::make_pair(D::RIGHT, "back"));
	transitions.add("back",               std::make_pair(D::DOWN,  "music_volume_down"));
	transitions.add("ok",                 std::make_pair(D::DOWN,  "music_volume_down"));
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
	lif::savePreferences(lif::preferencesPath);
}
