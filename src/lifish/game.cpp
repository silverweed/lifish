#include "game.hpp"
#include "LoopingMusic.hpp"
#include "collision_layers.hpp"
#include "conf/player.hpp"
#include "controls.hpp"
#include "HighScoreManager.hpp"
#include <array>
#include <cstring>
#include <sstream>
#include <string>

lif::HighScoreManager& lif::getHighScoreManager() {
	static lif::HighScoreManager highScoreMgr;
	return highScoreMgr;
}

bool lif::init() {
	if (!lif::initCore()) return false;

	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		if (sf::Joystick::isConnected(i))
			lif::controls::useJoystick[i] = i;
		else
			lif::controls::useJoystick[i] = -1;
	}

	// Setup collision layers
	lif::c_layers::init();

	lif::getHighScoreManager().loadHighScores();

	return true;
}

std::string lif::gameInfo() {
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
	ss << " by silverweed\r\n";

	return ss.str();
}
