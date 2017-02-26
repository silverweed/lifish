#pragma once

#include <vector>
#include <functional>

#define DEF_LOGIC(name) \
	void name(lif::Entity *e, lif::LevelManager& lm, std::vector<lif::Entity*>& tbspawned)

namespace lif {

class Entity;
class LevelManager;

/** Namespace containing game logic rules */
namespace game_logic {
	using GameLogicFunc = std::function<void(
			lif::Entity*,              // for-each loop variable
			lif::LevelManager&,        // LevelManager calling this function
			std::vector<lif::Entity*>& // vector of entities to be spawned after calling game logic
			)>;

	/** Make players drop bombs */
	DEF_LOGIC(bombDeployLogic);
	/** Make entities spawn other entities on certain conditions */
	DEF_LOGIC(spawningLogic);
	/** Make Scored entities give points when killed */
	DEF_LOGIC(scoredKillablesLogic);
	/** Trigger bonus effects */
	DEF_LOGIC(bonusGrabLogic);

	extern std::vector<GameLogicFunc> functions;
}

}
