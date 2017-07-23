#pragma once

#include "Direction.hpp"
#include <array>

#define NEW_DIRECTION(d) \
{ \
	moving->setDirection(d); \
	return; \
}

#define SAME_DIRECTION return;

#define HANDLE_NOT_MOVING \
	if (!moving->isMoving()) \
		SAME_DIRECTION

#define HANDLE_UNALIGNED \
	if (!entity.isAligned()) { \
		if (collider->collidesWithSolid()) \
			NEW_DIRECTION(lif::ai::adjust_prev_align(entity, *moving)) \
		else \
			SAME_DIRECTION \
	}

namespace lif {

class AxisSighted;
class LevelManager;
class AxisMoving;
class Entity;

namespace ai {

extern std::array<lif::Direction, 4> directions;

lif::Direction random_direction();

/** Selects a random direction where `moving` can go, choosing `opp` if
 * and only if no other viable direction is found.
 * If `forceChange` is true, avoid taking the same direction.
 */
lif::Direction select_random_viable(
		const lif::AxisMoving& moving,
		const lif::LevelManager& lm,
		const lif::Direction opp,
		bool forceChange = false);

lif::Direction seeing_player(const lif::LevelManager& lm, const lif::AxisSighted& sighted);

/** To be called when `entity` is colliding and is not aligned:
 *  sets `moving.prevAlign` to the tile it'd have reached if it hadn't collided,
 *  and returns oppositeDirection(moving.direction).
 */
lif::Direction adjust_prev_align(const lif::Entity& entity, lif::AxisMoving& moving);

} // end namespace ai

} // end namespace lif
