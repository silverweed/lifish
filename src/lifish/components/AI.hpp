#pragma once
/**
 * Headers for the AI functions
 */

#include <functional>
#include <exception>
#include <array>
#include "Component.hpp"
#include "Shooting.hpp"
#include "Direction.hpp"
#include "utils.hpp"

namespace Game {

class Enemy;
class LevelManager;

using AIBoundFunction = std::function<void(const Game::LevelManager&)>;
using AIFunction = std::function<AIBoundFunction(Game::Entity&)>;

constexpr unsigned short AI_FUNCTION_NUM = 6;

extern std::array<AIFunction, Game::AI_FUNCTION_NUM> ai_functions;

/**
 * An AI component gets updated at each frame to specify what its owner should
 * do next according to the current world state (represented by LevelManager).
 */
class AI : public Game::Component {
	AIBoundFunction func;
	Game::LevelManager *lm = nullptr;
	Game::Shooting *shooting = nullptr;

public:
	explicit AI(Game::Entity& owner, unsigned short aiNum)
		: Game::Component(owner) 
	{
		if (aiNum >= ai_functions.size())
			throw std::invalid_argument("Invalid aiNum passed: " + Game::to_string(aiNum));
		setAI(ai_functions[aiNum]);
		shooting = owner.get<Game::Shooting>();
	}

	void setAI(AIFunction newAI) {
		func = newAI(owner);
	}

	void setLevelManager(Game::LevelManager *_lm) {
		lm = _lm;
	}

	void update() override {
		if (lm == nullptr || (shooting != nullptr && shooting->isBlocked()))
			return;
		func(*lm);
	}
};

/** Choose a random direction; allow turning around */
AIBoundFunction ai_random(Game::Entity&);

/** Choose a random direction at each turn, but never turn around
 *  unless colliding.
 */
AIBoundFunction ai_random_forward(Game::Entity&);

/** Like ai_random_forward, but stick to the player while attacking. */
AIBoundFunction ai_random_forward_haunt(Game::Entity&);

/** Choose a random direction at each turn, unless player is in
 *  sight, in which case, follow him. (If more than 1 players are
 *  in sight, chase nearest one)
 */
AIBoundFunction ai_follow(Game::Entity& enemy);

/** Like ai_follow, but as soon as a player is seen, turn towards
 *  him, speed up and keep the same direction until an obstacle is
 *  found; at which point, return to normal speed and resume "classic"
 *  ai_follow behaviour.
 */
AIBoundFunction ai_follow_dash(Game::Entity& enemy);

/** Actively chase the nearest player */
AIBoundFunction ai_chase(Game::Entity& enemy);

}
