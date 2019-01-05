#pragma once

#include <vector>
#include <functional>
#include "BaseLevelManager.hpp"

#define DEF_LOGIC(name) \
	void name(lif::Entity& e, lif::BaseLevelManager& lm, std::vector<lif::Entity*>& tbspawned)

namespace lif {

class Entity;

/** Namespace containing game logic rules */
namespace game_logic {
	/** Make players drop bombs */
	DEF_LOGIC(bombDeployLogic);
	/** Make entities spawn other entities on certain conditions */
	DEF_LOGIC(spawningLogic);
	/** Make Scored entities give points when killed */
	DEF_LOGIC(scoredKillablesLogic);
	/** Trigger bonus effects */
	DEF_LOGIC(bonusGrabLogic);

	extern std::vector<lif::BaseLevelManager::GameLogicFunc> functions;
}

}
