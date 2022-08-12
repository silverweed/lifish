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
		if (collider->collidesWithSolid()) { \
			NEW_DIRECTION(lif::ai::adjustPrevAlign(entity, *moving)) \
		} else { \
			SAME_DIRECTION \
		} \
	}

namespace lif {

class AxisSighted;
class LevelManager;
class AxisMoving;
class Entity;

namespace ai {

extern std::array<lif::Direction, 4> directions;

lif::Direction random_direction();

enum SelectRandomViableFlags {
	FORCE_CHANGE             = 1,
	OPPOSITE_NOT_LAST_CHOICE = 2,
};

/** Selects a random direction where `moving` can go, choosing `opp` if
 * and only if no other viable direction is found or 'OPPOSITE_NOT_LAST_CHOICE' is set.
 * If `FORCE_CHANGE` is set, avoid taking the same direction.
 */
lif::Direction selectRandomViable(
		const lif::AxisMoving& moving,
		const lif::LevelManager& lm,
		const lif::Direction opp,
		int flags = 0);

lif::Direction seeingPlayer(const lif::LevelManager& lm, const lif::AxisSighted& sighted);

/** To be called when `entity` is colliding and is not aligned:
 *  sets `moving.prevAlign` to the tile it'd have reached if it hadn't collided,
 *  and returns oppositeDirection(moving.direction).
 */
lif::Direction adjustPrevAlign(const lif::Entity& entity, lif::AxisMoving& moving);

} // end namespace ai

} // end namespace lif
