#include "Game.hpp"
#include "GameCache.hpp"
#include <cstring>
#include <iostream>

#if defined(IS_POSIX)
#	include <unistd.h>
#elif defined(IS_WINDOWS)
#	include <stdlib.h>
#elif defined(IS_APPLE)
#	include <mach-o/dyld.h>
#endif

namespace Game {
	char pwd[PWD_BUFSIZE];
	GameCache cache;
}

using Game::pwd;

bool Game::init() {
	// Setup pwd variable
	pwd[0] = '\0';
#if defined(IS_POSIX)
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

#elif defined(IS_WINDOWS)
	if (_get_pgmptr(pwd) != 0)
		return false;

#elif defined(IS_APPLE)
	auto bufsz = static_cast<uint32_t>(Game::PWD_BUFSIZE);
	if (_NSGetExecutablePath(pwd, &bufsz) != 0)
		return false;

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
