#include "preferences_persistence.hpp"
#include "Options.hpp"
#include "language.hpp"
#include "controls.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <tinyjson.h>

using tinyjson::json;

static json videoModeToJson(const sf::VideoMode& videoMode) {
	return tinyjson::json_array {
		json(int(videoMode.width)), json(int(videoMode.height)), json(int(videoMode.bitsPerPixel))
	};
}

static sf::VideoMode videoModeFromJson(const json& json) {
	const tinyjson::json_array& a = json.get_array();
	sf::VideoMode vm;
	vm.width = a[0].get_integer();
	vm.height = a[1].get_integer();
	vm.bitsPerPixel = a[2].get_integer();
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
	json out = tinyjson::json_object {};

	out.add_member("nPlayers", lif::options.nPlayers);
	out.add_member("musicVolume", static_cast<int>(100 * lif::options.musicVolume));
	out.add_member("soundVolume", static_cast<int>(100 * lif::options.soundsVolume));
	out.add_member("soundMuted", lif::options.soundsMute);
	out.add_member("fullscreen", lif::options.fullscreen);
	out.add_member("videoMode", videoModeToJson(lif::options.videoMode));
	out.add_member("showFPS", lif::options.showFPS);
	out.add_member("showGameTimer", lif::options.showGameTimer);
	out.add_member("vsync", lif::options.vsync);
	out.add_member("fpsLimit", lif::options.framerateLimit);
	out.add_member("lang", lif::strFromLang(lif::curLang));

	out.add_member("controls", tinyjson::json_array {});
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		out["controls"].add_element(tinyjson::json_object {
			{ "up", lif::controls::players[i][lif::controls::CTRL_UP] },
			{ "down", lif::controls::players[i][lif::controls::CTRL_DOWN] },
			{ "right", lif::controls::players[i][lif::controls::CTRL_RIGHT] },
			{ "left", lif::controls::players[i][lif::controls::CTRL_LEFT] },
			{ "bomb", lif::controls::players[i][lif::controls::CTRL_BOMB] },
			{ "joyBomb", int(lif::controls::joystickBombKey[i]) },
			{ "useJoystick", lif::controls::useJoystick[i] },
		});
	}

	file << out.to_string() << "\n";

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
		json in = tinyjson::parser::parse(prefsStr.c_str());

		lif::options.nPlayers = lif::clamp<int>(in["nPlayers"].get_integer(), 1, lif::MAX_PLAYERS);
		lif::options.musicVolume = in["musicVolume"].get_integer() * 0.01f;
		lif::options.soundsVolume = in["soundVolume"].get_integer() * 0.01f;
		lif::options.soundsMute = in["soundMuted"].get_bool();
		lif::options.fullscreen = in["fullscreen"].get_bool();
		lif::options.videoMode = videoModeFromJson(in["videoMode"]);
		lif::options.showFPS = in["showFPS"].get_bool();
		lif::options.showGameTimer = in["showGameTimer"].get_bool();
		lif::options.vsync = in["vsync"].get_bool();
		lif::options.framerateLimit = in["fpsLimit"].get_integer();
		lif::curLang = lif::langFromStr(in["lang"].get_string());

		for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
			const auto& ctrls = in["controls"][i];
			lif::controls::players[i][lif::controls::CTRL_UP] = static_cast<sf::Keyboard::Key>(ctrls["up"].get_integer());
			lif::controls::players[i][lif::controls::CTRL_DOWN] = static_cast<sf::Keyboard::Key>(ctrls["down"].get_integer());
			lif::controls::players[i][lif::controls::CTRL_LEFT] = static_cast<sf::Keyboard::Key>(ctrls["left"].get_integer());
			lif::controls::players[i][lif::controls::CTRL_RIGHT] = static_cast<sf::Keyboard::Key>(ctrls["right"].get_integer());
			lif::controls::players[i][lif::controls::CTRL_BOMB] = static_cast<sf::Keyboard::Key>(ctrls["bomb"].get_integer());
			lif::controls::joystickBombKey[i] = static_cast<sf::Keyboard::Key>(ctrls["joyBomb"].get_integer());
			lif::controls::useJoystick[i] = static_cast<sf::Keyboard::Key>(ctrls["useJoystick"].get_integer());
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
