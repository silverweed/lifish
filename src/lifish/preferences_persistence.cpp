#include "preferences_persistence.hpp"
#include "Options.hpp"
#include "language.hpp"
#include "controls.hpp"
#include "game.hpp"
#include "json.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

static inline std::string getCompleteFname(const char *fname) {
	std::stringstream ss;
	ss << lif::pwd << lif::DIRSEP << fname;
	return ss.str();
}

void lif::savePreferences(const char *fname) {
	const auto filename = getCompleteFname(fname);

	std::ofstream file(filename);
	if (!file) {
		std::cerr << "[ WARNING ] could not save preferences in " << filename << "!\n";
		return;
	}

	// Serialize options into file
	json out;

	out["nPlayers"] = lif::options.nPlayers;
	out["musicVolume"] = static_cast<int>(100 * lif::options.musicVolume);
	out["soundVolume"] = static_cast<int>(100 * lif::options.soundsVolume);
	out["soundMuted"] = lif::options.soundsMute;
	out["fullscreen"] = lif::options.fullscreen;
	out["videoMode"] = lif::options.videoMode;
	out["showFPS"] = lif::options.showFPS;
	out["showGameTimer"] = lif::options.showGameTimer;
	out["vsync"] = lif::options.vsync;
	out["fpsLimit"] = lif::options.framerateLimit;
	out["lang"] = lif::strFromLang(lif::curLang);

	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		out["controls"][i] = {
			{ "up", lif::controls::players[i][lif::controls::CTRL_UP] },
			{ "down", lif::controls::players[i][lif::controls::CTRL_DOWN] },
			{ "right", lif::controls::players[i][lif::controls::CTRL_RIGHT] },
			{ "left", lif::controls::players[i][lif::controls::CTRL_LEFT] },
			{ "bomb", lif::controls::players[i][lif::controls::CTRL_BOMB] },
			{ "joyBomb", lif::controls::joystickBombKey[i] },
			{ "useJoystick", lif::controls::useJoystick[i] },
		};
	}

	file << out.dump(8) << "\n";
}

void lif::loadPreferences(const char *fname) {
	const auto filename = getCompleteFname(fname);

	std::ifstream file(filename);
	if (!file) {
		std::cerr << "[ INFO ] could not load preferences file " << filename << ".\n";
		return;
	}

	// TODO: validate input
	try {
		json in = json::parse(file);

		lif::options.nPlayers = lif::clamp<int>(in["nPlayers"], 1, lif::MAX_PLAYERS);
		lif::options.musicVolume = in["musicVolume"].get<int>() * 0.01f;
		lif::options.soundsVolume = in["soundVolume"].get<int>() * 0.01f;
		lif::options.soundsMute = in["soundMuted"];
		lif::options.fullscreen = in["fullscreen"];
		lif::options.videoMode = in["videoMode"];
		lif::options.showFPS = in["showFPS"];
		lif::options.showGameTimer = in["showGameTimer"];
		lif::options.vsync = in["vsync"];
		lif::options.framerateLimit = in["fpsLimit"];
		lif::curLang = lif::langFromStr(in["lang"]);

		for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
			const auto& ctrls = in["controls"][i];
			lif::controls::players[i][lif::controls::CTRL_UP] = ctrls["up"];
			lif::controls::players[i][lif::controls::CTRL_DOWN] = ctrls["down"];
			lif::controls::players[i][lif::controls::CTRL_LEFT] = ctrls["left"];
			lif::controls::players[i][lif::controls::CTRL_RIGHT] = ctrls["right"];
			lif::controls::players[i][lif::controls::CTRL_BOMB] = ctrls["bomb"];
			lif::controls::joystickBombKey[i] = ctrls["joyBomb"];
			lif::controls::useJoystick[i] = ctrls["useJoystick"];
		}

	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}
