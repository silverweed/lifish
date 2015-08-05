#pragma once
/**
 * Headers for the AI functions
 */

#include <functional>
#include <array>
#include "LevelRenderer.hpp"

namespace Game {

class Enemy;

using AIBoundFunction = std::function<Game::Direction(const Game::LevelRenderer *const)>;
using AIFunction = std::function<AIBoundFunction(Game::Enemy *const)>;

/** Choose a random direction; allow turning around */
AIBoundFunction ai_random(Game::Enemy *const);

/** Choose a random direction at each turn, but never turn around
 *  unless colliding.
 */
AIBoundFunction ai_random_forward(Game::Enemy *const);

/** Like ai_random_forward, but stick to the player while attacking. */
AIBoundFunction ai_random_forward_haunt(Game::Enemy *const);

/** Choose a random direction at each turn, unless player is in
 *  sight, in which case, follow him. (If more than 1 players are
 *  in sight, chase nearest one)
 */
AIBoundFunction ai_follow(Game::Enemy *const enemy);

/** Like ai_follow, but as soon as a player is seen, turn towards
 *  him, speed up and keep the same direction until an obstacle is
 *  found; at which point, return to normal speed and resume "classic"
 *  ai_follow behaviour.
 */
AIBoundFunction ai_follow_dash(Game::Enemy *const enemy);

/** Actively chase the nearest player */
AIBoundFunction ai_chase(Game::Enemy *const enemy);

static std::array<AIFunction, 6> ai_functions = {{
	ai_random,
	ai_random_forward,
	ai_random_forward_haunt,
	ai_follow,
	ai_follow_dash,
	ai_chase
}};

}
