#pragma once

#include "Component.hpp"
#include <array>
#include <functional>

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
	const lif::LevelManager *lm = nullptr;
	lif::Moving *moving = nullptr;

public:
	explicit AI(lif::Entity& owner, AIFunction ai);

	void setAI(AIFunction newAI);

	void setLevelManager(const lif::LevelManager *_lm);
	const lif::LevelManager* getLevelManager() const { return lm; }

	lif::Entity* init() override;
	void update() override;
};

}
