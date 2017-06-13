#pragma once

#include "utils.hpp"

namespace lif {

namespace c_layers {

enum Layer : unsigned {
	DEFAULT,
	IGNORE_ALL, // similar to deactivate the collider, but keep collision with level bounds
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
	ENEMY_EXPLOSIONS,
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
