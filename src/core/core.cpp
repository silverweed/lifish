#include "core.hpp"
#include "GameCache.hpp"
#include "Options.hpp"
#include "preferences_persistence.hpp"
#include "Time.hpp"
#include <chrono>
#include <cstring>
#ifndef RELEASE
#	include "DebugPainter.hpp"
#	include "FadeoutTextManager.hpp"
#endif

#if defined(SFML_SYSTEM_MACOS)
#	include "MacPaths.h"
#elif defined(__unix__)
#	include <cstdlib>
#	include <sys/stat.h>
#	include <unistd.h>
#elif defined(SFML_SYSTEM_WINDOWS)
#	include <Windows.h>
#else
#	error "Unsupported platform."
#endif

// Define extern variables of core.hpp
char lif::pwd[lif::PWD_BUFSIZE];
std::string lif::assetDir;
std::string lif::saveDir;
std::string lif::prefsDir;
std::string lif::preferencesPath;
lif::GameCache lif::cache;
std::default_random_engine lif::rng;
lif::Options lif::options;
bool lif::terminated = false;
int lif::exitCode = 0;
lif::Time<std::chrono::high_resolution_clock> lif::time;
lif::MusicManager *lif::musicManager = nullptr;
#ifndef RELEASE
lif::debug::DebugPainter *lif::debugPainter = nullptr;
lif::debug::FadeoutTextManager *lif::fadeoutTextMgr = nullptr;
#endif

static void _initOptions() {
	using lif::options;

	// Setup default options
	options.musicVolume = 100;
	options.soundsVolume = 100;
	options.soundsMute = false;
	options.vsync = true;
	options.framerateLimit = 60;
	options.windowSize = sf::Vector2u(640, 480);
}

static bool _initPwd() {
	using lif::pwd;
	using lif::DIRSEP;

	// Setup pwd variable cross-platform
	pwd[0] = '\0';

#if defined(SFML_SYSTEM_WINDOWS)
	GetModuleFileName(NULL, pwd, lif::PWD_BUFSIZE);

#elif defined(__unix__)
	ssize_t bytes = 0;
	if (access("/proc/self/exe", F_OK) != -1) {
		// Linux
		bytes = readlink("/proc/self/exe", pwd, lif::PWD_BUFSIZE-1);

	} else if (access("/proc/curproc/file", F_OK) != -1) {
		// BSD
		bytes = readlink("/proc/curproc/file", pwd, lif::PWD_BUFSIZE-1);
	}

	if (bytes < 1) return false;
	pwd[bytes] = '\0';
#endif

#if !defined(SFML_SYSTEM_MACOS)
	// On macOS, the path to the executable is not used
	int len = strlen(pwd);
	if (len < 1)
		return false;

	// strip executable name
	for (int i = len-1; i > -1; --i) {
		if (pwd[i] == DIRSEP) {
			pwd[i] = '\0';
			break;
		}
	}
#endif

	return true;
}

bool lif::initCore() {
	// Seed the random engine
	try {
		rng.seed(std::random_device{}());
	} catch (const std::exception&) {
		rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
	}
	_initOptions();
	if (!_initPwd())
		return false;

	// Fill the assetDir and saveDir variables once and for all
#if defined(SFML_SYSTEM_MACOS)
	// In addition to saved games, saveDir is also used to save preferences
	assetDir = bundleResourcesPath() + DIRSEP;
	saveDir = applicationSupportPath() + DIRSEP + "BOOM Remake" + DIRSEP;
	preferencesPath = saveDir + std::string(PREFERENCES_SAVE_FILE_NAME);

#elif defined(__unix__)
	const char * xdg_data_home = std::getenv("XDG_DATA_HOME");
	const char * xdg_config_home = std::getenv("XDG_CONFIG_HOME");
	const char * home = std::getenv("HOME");

	std::stringstream ss;
	// Used for the AppImage distribution
	ss << pwd << "/../../assets/";

	struct stat result;
	int err = stat(ss.str().c_str(), &result);
	if (err != 0 || !S_ISDIR(result.st_mode)) {
		ss.str("");
		ss << pwd << "/assets/";
	}

	assetDir = ss.str();

	if (xdg_data_home) {
		saveDir = std::string(xdg_data_home) + "/lifish/";
	} else {
		saveDir = std::string(home) + "/.local/share/lifish/";
	}

	if (xdg_config_home) {
		prefsDir = std::string(xdg_config_home) + "/lifish/";
	} else {
		prefsDir = std::string(home) + "/.config/lifish/";
	}

	preferencesPath = prefsDir + std::string(PREFERENCES_SAVE_FILE_NAME);

#else
	std::stringstream ss;
	ss << pwd << DIRSEP << "assets" << DIRSEP;
	assetDir = ss.str();

	ss.str("");
	ss << pwd << DIRSEP << "saves" << DIRSEP;
	saveDir = ss.str();

	preferencesPath = std::string(pwd) + DIRSEP + std::string(PREFERENCES_SAVE_FILE_NAME);
#endif

	return true;
}
