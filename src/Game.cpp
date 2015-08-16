#include "Game.hpp"
#include <cstring>
#include <iostream>
#include <array>

#if defined(SFML_SYSTEM_MACOS)
#	include <mach-o/dyld.h>
#elif defined(__unix__)
#	include <unistd.h>
#elif defined(SFML_SYSTEM_WINDOWS)
#	include <Windows.h> 
#else
#	error "Unsupported platform."
#endif

// Define extern variables of Game.hpp
char Game::pwd[Game::PWD_BUFSIZE];
std::array<unsigned int, Game::MAX_PLAYERS> Game::score;
Game::GameCache Game::cache;
std::default_random_engine Game::rng;

using Game::pwd;

bool Game::init() {
	// Setup pwd variable
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
