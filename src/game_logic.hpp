#pragma once

#include "Entity.hpp"
#include "LevelManager.hpp"
#include <vector>

#define DEF_LOGIC(name) \
	void name(Game::Entity *e, Game::LevelManager& lm, \
			std::list<Game::Entity*>& tbspawned, std::list<Game::Entity*>& tbkilled)

namespace Game {

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
	/** Make explosions destroy stuff */
	//DEF_LOGIC(explosionDamageLogic);

	extern std::vector<GameLogicFunc> functions;
}

}
