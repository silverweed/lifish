#include "game.hpp"
#include "LoopingMusic.hpp"
#include "controls.hpp"
#include "collision_layers.hpp"
#include <cstring>
#include <iostream>
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
