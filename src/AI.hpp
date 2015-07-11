#pragma once
/**
 * Headers for the AI functions
 */

#include <functional>
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

/** Choose a random direction at each turn, unless player is in
 *  sight, in which case, follow him. (If more than 1 players are
 *  in sight, chase nearest one)
 */
AIBoundFunction ai_follow(Game::Enemy *const enemy);

}
