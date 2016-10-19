#pragma once

#include <functional>
#include <array>
#include "Component.hpp"

namespace Game {

class LevelManager;
class Moving;

using AIBoundFunction = std::function<void(const Game::LevelManager&)>;
using AIFunction = std::function<AIBoundFunction(Game::Entity&)>;

/**
 * An AI component gets updated at each frame to specify what its owner should
 * do next according to the current world state (represented by LevelManager).
 */
class AI : public Game::Component {
	AIFunction ai;
	AIBoundFunction func;
	Game::LevelManager *lm = nullptr;
	Game::Moving *moving = nullptr;

public:
	explicit AI(Game::Entity& owner, AIFunction ai);

	void setAI(AIFunction newAI);

	void setLevelManager(Game::LevelManager *_lm);

	Game::Entity* init() override; 
	void update() override;
};

}
