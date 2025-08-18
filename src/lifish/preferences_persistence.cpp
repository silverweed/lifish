#include "preferences_persistence.hpp"
#include "Options.hpp"
#include "language.hpp"
#include "controls.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include "json.hpp"

using namespace picojson;

static value videoModeToJson(const sf::VideoMode& videoMode) {
	return value(value::array {
		value(int64_t(videoMode.width)), value(int64_t(videoMode.height)), value(int64_t(videoMode.bitsPerPixel))
	});
}

static sf::VideoMode videoModeFromJson(const value& json) {
	const auto& a = json.get<array>();
	sf::VideoMode vm;
	vm.width = a[0].get<int64_t>();
	vm.height = a[1].get<int64_t>();
	vm.bitsPerPixel = a[2].get<int64_t>();
	return vm;
}

void lif::savePreferences(std::string fpath) {
#if defined(SFML_SYSTEM_MACOS)
	if (!lif::createDirIfNotExisting(lif::saveDir)) {
		return;
	}
#elif defined(__unix__)
	if (!lif::createDirIfNotExisting(lif::prefsDir)) {
		return;
	}
#endif

	std::ofstream file(fpath);
	if (!file) {
		std::cerr << "[ WARNING ] could not save preferences in " << fpath << "!\n";
		return;
	}

	// Serialize options into file
	auto out = picojson::value::object {};

	out["nPlayers"] = value(int64_t(lif::options.nPlayers));
	out["musicVolume"] = value(int64_t(100 * lif::options.musicVolume));
	out["soundVolume"] = value(int64_t(100 * lif::options.soundsVolume));
	out["soundMuted"] = value(lif::options.soundsMute);
	out["fullscreen"] = value(lif::options.fullscreen);
	out["videoMode"] = value(videoModeToJson(lif::options.videoMode));
	out["showFPS"] = value(lif::options.showFPS);
	out["showGameTimer"] = value(lif::options.showGameTimer);
	out["vsync"] = value(lif::options.vsync);
	out["fpsLimit"] = value(int64_t(lif::options.framerateLimit));
	out["lang"] = value(lif::strFromLang(lif::curLang));

	out["controls"] = value(value::array {});
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		out["controls"].get<array>().push_back(value(value::object {
			{ "up", value(int64_t(lif::controls::players[i][lif::controls::CTRL_UP])) },
			{ "down", value(int64_t(lif::controls::players[i][lif::controls::CTRL_DOWN])) },
			{ "right", value(int64_t(lif::controls::players[i][lif::controls::CTRL_RIGHT])) },
			{ "left", value(int64_t(lif::controls::players[i][lif::controls::CTRL_LEFT])) },
			{ "bomb", value(int64_t(lif::controls::players[i][lif::controls::CTRL_BOMB])) },
			{ "joyBomb", value(int64_t(int(lif::controls::joystickBombKey[i]))) },
			{ "useJoystick", value(int64_t(lif::controls::useJoystick[i])) },
		}));
	}

	file << value(out).serialize() << "\n";

	std::cerr << "[ INFO ] saved preferences in " << fpath << ".\n";
}

void lif::loadPreferences(std::string fpath) {
	std::string prefsStr = lif::readEntireFile(fpath);
	if (prefsStr.empty()) {
		std::cerr << "[ INFO ] could not load preferences file " << fpath << ".\n";
		return;
	}

	// TODO: validate input
	try {
		value inJson;
		auto err = picojson::parse(inJson, prefsStr);
		if (!err.empty())
			throw std::logic_error(err);

		auto in = inJson.get<object>();

		lif::options.nPlayers = lif::clamp<int>(in["nPlayers"].get<int64_t>(), 1, lif::MAX_PLAYERS);
		lif::options.musicVolume = in["musicVolume"].get<int64_t>() * 0.01f;
		lif::options.soundsVolume = in["soundVolume"].get<int64_t>() * 0.01f;
		lif::options.soundsMute = in["soundMuted"].get<bool>();
		lif::options.fullscreen = in["fullscreen"].get<bool>();
		lif::options.videoMode = videoModeFromJson(in["videoMode"]);
		lif::options.showFPS = in["showFPS"].get<bool>();
		lif::options.showGameTimer = in["showGameTimer"].get<bool>();
		lif::options.vsync = in["vsync"].get<bool>();
		lif::options.framerateLimit = in["fpsLimit"].get<int64_t>();
		lif::curLang = lif::langFromStr(in["lang"].get<std::string>());

		for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
			const auto& ctrls = in["controls"].get<array>()[i].get<object>();
			lif::controls::players[i][lif::controls::CTRL_UP] = static_cast<sf::Keyboard::Key>(get_or<int64_t>(ctrls, "up"));
			lif::controls::players[i][lif::controls::CTRL_DOWN] = static_cast<sf::Keyboard::Key>(get_or<int64_t>(ctrls, "down"));
			lif::controls::players[i][lif::controls::CTRL_LEFT] = static_cast<sf::Keyboard::Key>(get_or<int64_t>(ctrls, "left"));
			lif::controls::players[i][lif::controls::CTRL_RIGHT] = static_cast<sf::Keyboard::Key>(get_or<int64_t>(ctrls, "right"));
			lif::controls::players[i][lif::controls::CTRL_BOMB] = static_cast<sf::Keyboard::Key>(get_or<int64_t>(ctrls, "bomb"));
			lif::controls::joystickBombKey[i] = static_cast<sf::Keyboard::Key>(get_or<int64_t>(ctrls, "joyBomb"));
			lif::controls::useJoystick[i] = static_cast<sf::Keyboard::Key>(get_or<int64_t>(ctrls, "useJoystick"));
		}

	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	// Validate joysticks
	for (auto& useJoy : lif::controls::useJoystick) {
		if (useJoy >= 0 && !sf::Joystick::isConnected(useJoy))
			useJoy = -1;
	}
}
