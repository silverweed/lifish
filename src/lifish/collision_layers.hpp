#pragma once

#include "utils.hpp"

namespace lif {

namespace c_layers {

enum Layer : unsigned {
	DEFAULT,
	PLAYERS,
	ENEMIES,
	ENEMIES_IGNORE_BREAKABLES,
	BOSSES,
	ENEMY_BULLETS,
	BOSS_BULLETS,
	BREAKABLES,
	UNBREAKABLES,
	TRANSP_WALLS,
	TELEPORTS,
	EXPLOSIONS,
	BOMBS,
	GRABBABLE,
	N_LAYERS
};

extern Matrix<bool, c_layers::N_LAYERS, c_layers::N_LAYERS> collide, // whether levels "see" each other
                                                            solid;   // whether levels are solid for each other

/** To be called by lif::init() */
void init();

}

}
