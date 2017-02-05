#include "game.hpp"
#include "LoopingMusic.hpp"
#include "controls.hpp"
#include "collision_layers.hpp"
#include <string>
#include <cstring>
#include <sstream>
#include <array>
#include "conf/player.hpp"

std::array<unsigned int, lif::MAX_PLAYERS> lif::score;
std::array<unsigned short, lif::MAX_PLAYERS> lif::playerContinues;

bool lif::init() {
	if (!lif::initCore()) return false;
	lif::playerContinues.fill(lif::conf::player::INITIAL_CONTINUES);
	lif::controls::useJoystick.fill(-1);

	// Setup collision layers
	lif::c_layers::init();

	return true;
}

std::string lif::game_info() {
	std::stringstream ss;
	ss << "lifish v." VERSION " rev." COMMIT;
#ifdef RELEASE
	ss << " RELEASE";
#endif
#ifndef ARCH
	ss << " (unknown arch)";
#else
	ss << " (" ARCH " bit)";
#endif
	ss << " by Giacomo Parolini\r\n";
#ifdef HAVE_NFD
	ss << "    | NFD support: yes\r\n";
#elif !defined(SFML_SYSTEM_WINDOWS)
	ss << "    | NFD support: no\r\n";
#endif
#ifdef MULTITHREADED 
	ss << "    | Multithreaded: yes\r\n";
#else
	ss << "    | Multithreaded: no\r\n";
#endif
	return ss.str();
}
