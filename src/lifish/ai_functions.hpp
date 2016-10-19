#pragma once
/**
 * Headers for the AI functions
 */

#include "AI.hpp"

namespace Game {

constexpr unsigned short AI_FUNCTIONS_NUM = 6;

extern std::array<Game::AIFunction, Game::AI_FUNCTIONS_NUM> ai_functions;

/** Choose a random direction; allow turning around */
Game::AIBoundFunction ai_random(Game::Entity&);

/** Choose a random direction at each turn, but never turn around
 *  unless colliding.
 */
Game::AIBoundFunction ai_random_forward(Game::Entity&);

/** Like ai_random_forward, but stick to the player while attacking. */
Game::AIBoundFunction ai_random_forward_haunt(Game::Entity&);

/** Choose a random direction at each turn, unless player is in
 *  sight, in which case, follow him. (If more than 1 players are
 *  in sight, chase nearest one)
 */
Game::AIBoundFunction ai_follow(Game::Entity& enemy);

/** Like ai_follow, but as soon as a player is seen, turn towards
 *  him, speed up and keep the same direction until an obstacle is
 *  found; at which point, return to normal speed and resume "classic"
 *  ai_follow behaviour.
 */
Game::AIBoundFunction ai_follow_dash(Game::Entity& enemy);

/** Actively chase the nearest player */
Game::AIBoundFunction ai_chase(Game::Entity& enemy);

} // end namespace Game
