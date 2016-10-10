#pragma once

#include <vector>
#include <list>
#include <functional>

#define DEF_LOGIC(name) \
	void name(Game::Entity *e, Game::LevelManager& lm, \
			std::list<Game::Entity*>& tbspawned, std::list<Game::Entity*>& tbkilled)

namespace Game {

class Entity;
class LevelManager;

/** Namespace containing game logic rules */
namespace Logic {
	using GameLogicFunc = std::function<void(
			Game::Entity*,             // for-each loop variable 
			Game::LevelManager&,       // LevelManager calling this function
			std::list<Game::Entity*>&, // list of entities to be spawned after calling game logic
			std::list<Game::Entity*>&  // list of entities to be removed after calling game logic
			)>;

	/** Make bombs explode */
	DEF_LOGIC(bombExplosionLogic);
	/** Make players drop bombs */
	DEF_LOGIC(bombDeployLogic);
	/** Make entities spawn other entities on certain conditions */
	DEF_LOGIC(spawningLogic);
	/** Make Scored entities give points when killed */
	DEF_LOGIC(scoredKillablesLogic);
	/** Make entities shoot */
	DEF_LOGIC(shootLogic);
	/** Trigger bonus effects */
	DEF_LOGIC(bonusGrabLogic);

	extern std::vector<GameLogicFunc> functions;
}

}
