#include "core.hpp"
#include "Options.hpp"
#include "GameCache.hpp"
#include <cstring>

#if defined(SFML_SYSTEM_MACOS)
#	include <mach-o/dyld.h>
#elif defined(__unix__)
#	include <unistd.h>
#elif defined(SFML_SYSTEM_WINDOWS)
#	include <Windows.h> 
#else
#	error "Unsupported platform."
#endif

// Define extern variables of core.hpp
char Game::pwd[Game::PWD_BUFSIZE];
Game::GameCache Game::cache;
std::default_random_engine Game::rng;
Game::Options Game::options;
bool Game::terminated = false;
int Game::exitCode = 0;
Game::MusicManager *Game::musicManager = nullptr;

static void _initOptions() {
	using Game::options;

	// Setup default options
	options.musicVolume = 100;
	options.soundsVolume = 100;
	options.soundsMute = false;
	options.showFPS = false;
	options.vsync = true;
	options.framerateLimit = 60;
	options.windowSize = sf::Vector2u(640, 480);
}

static bool _initPwd() {
	using Game::pwd;
	using Game::DIRSEP;

	// Setup pwd variable cross-platform
	pwd[0] = '\0';

#if defined(SFML_SYSTEM_WINDOWS)
	GetModuleFileName(NULL, pwd, Game::PWD_BUFSIZE);

#elif defined(SFML_SYSTEM_MACOS)
	auto bufsz = static_cast<uint32_t>(Game::PWD_BUFSIZE);
	if (_NSGetExecutablePath(pwd, &bufsz) != 0)
		return false;

#elif defined(__unix__)
	ssize_t bytes = 0;
	if (access("/proc/self/exe", F_OK) != -1) {
		// Linux
		bytes = readlink("/proc/self/exe", pwd, Game::PWD_BUFSIZE-1);

	} else if (access("/proc/curproc/file", F_OK) != -1) {
		// BSD
		bytes = readlink("/proc/curproc/file", pwd, Game::PWD_BUFSIZE-1);
	}

	if (bytes < 1) return false;
	pwd[bytes] = '\0';
#endif
	
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

	return true;
}

bool Game::initCore() {
	_initOptions();
	return _initPwd();
}
