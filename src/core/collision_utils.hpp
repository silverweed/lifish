#pragma once

#include "Direction.hpp"

namespace Game {

class Collider;
class CompoundCollider;
class AxisMoving;

namespace CollisionUtils {

/** Checks if `cld1` and `cld2` collide, given that the owner of `cld1` has direction `dir`. */
bool collide(const Game::Collider& cld1, const Game::Collider& cld2, const Game::Direction dir);

/** Checks collision between a CompoundCollider and a Collider */
bool check_compound(const Game::CompoundCollider& cc1, 
		const Game::Collider& cld2, const Game::Direction dir);

/** Checks if `cld` is at the level limit. */
bool is_at_boundaries(const Game::Collider& cld, const Game::AxisMoving *const am);

/** Checks if `ocld` is along the forward direction of `cld` */
bool direction_is_viable(const Game::Collider& cld, 
		const Game::AxisMoving& moving, const Game::Collider& ocld);

}

}
