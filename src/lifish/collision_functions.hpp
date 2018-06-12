#pragma once

#include "Collider.hpp"

namespace lif {

enum CollisionFuncOptions : unsigned {
	/** Only be hit by explosion originated at distance 1 from entity */
	CFO_ONLY_ADJACENT = 1 << 0,
	/** Only receive 1 dmg per hit */
	CFO_1_DMG_PER_HIT = 1 << 1,
};

/** Reusable callback for a Collider that gets hurt by an explosion.
 *  Required: owner must have a Lifed and a Killable.
 *  Optional: owner may have a Scored.
 */
lif::Collider::CollisionFunc hurtByExplosions(lif::Entity& e, unsigned opts = 0);

}
