#pragma once

#include <functional>
#include <array>
#include "Component.hpp"

namespace lif {

class LevelManager;
class Moving;

using AIBoundFunction = std::function<void(const lif::LevelManager&)>;
using AIFunction = std::function<AIBoundFunction(lif::Entity&)>;

/**
 * An AI component gets updated at each frame to specify what its owner should
 * do next according to the current world state (represented by LevelManager).
 */
class AI : public lif::Component {
	AIFunction ai;
	AIBoundFunction func;
	lif::LevelManager *lm = nullptr;
	lif::Moving *moving = nullptr;

public:
	explicit AI(lif::Entity& owner, AIFunction ai);

	void setAI(AIFunction newAI);

	void setLevelManager(lif::LevelManager *_lm);
	const lif::LevelManager* getLevelManager() const { return lm; }

	lif::Entity* init() override;
	void update() override;
};

}
