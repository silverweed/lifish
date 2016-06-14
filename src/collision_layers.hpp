#pragma once

#include "utils.hpp"

namespace Game {

namespace Layers {

enum Layer : unsigned short {
	DEFAULT       = 0,
	PLAYERS       = 1,
	ENEMIES       = 2,
	BOSSES        = 3,
	ENEMY_BULLETS = 4,
	BOSS_BULLETS  = 5,
	WALLS         = 6,
	N_LAYERS      = 7
};

extern Matrix<bool, Layers::N_LAYERS, Layers::N_LAYERS> collide, // whether levels "see" each other
                                                        solid;   // whether levels are solid for each other

/** To be called by Game::init() */
void init();

}

}
