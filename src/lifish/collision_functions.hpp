#pragma once

#include "Collider.hpp"

namespace Game {

enum CollisionFuncOptions : unsigned {
	/** Take only the first hit from any explosion */
	CFO_TAKE_SINGLE_HIT = 1,
	/** Only be hit by explosion originated at distance 1 from entity */
	CFO_ONLY_ADJACENT   = 1 << 1,
};

/** Reusable callback for a Collider that gets hurt by an explosion.
 *  Required: owner must have a Lifed and a Killable.
 *  Optional: owner may have a Scored.
 */
Game::Collider::CollisionFunc hurtByExplosions(Game::Entity& e, unsigned opts = 0);

}
