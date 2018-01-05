#pragma once
/**
 * Headers for the AI functions
 */

#include "AI.hpp"

namespace lif {

constexpr unsigned short AI_FUNCTIONS_NUM = 6;

extern const std::array<lif::AIFunction, lif::AI_FUNCTIONS_NUM> ai_functions;

/** Choose a random direction; allow turning around */
lif::AIBoundFunction ai_random(lif::Entity&);

/** Choose a random direction at each turn, but never turn around
 *  unless colliding.
 */
lif::AIBoundFunction ai_random_forward(lif::Entity&);

/** Like ai_random_forward, but stick to the player while attacking. */
lif::AIBoundFunction ai_random_forward_haunt(lif::Entity&);

/** Choose a random direction at each turn, unless player is in
 *  sight, in which case, follow him. (If more than 1 players are
 *  in sight, chase nearest one)
 */
lif::AIBoundFunction ai_follow(lif::Entity& enemy);

/** Like ai_follow, but as soon as a player is seen, turn towards
 *  him, speed up and keep the same direction until an obstacle is
 *  found; at which point, return to normal speed and resume "classic"
 *  ai_follow behaviour.
 */
lif::AIBoundFunction ai_follow_dash(lif::Entity& enemy);

/** Actively chase the nearest player */
lif::AIBoundFunction ai_chase(lif::Entity& enemy);

} // end namespace lif
