#pragma once

#include "Entity.hpp"
#include "LevelManager.hpp"
#include <vector>

namespace Game {

namespace Logic {
	using GameLogicFunc = std::function<void(Game::Entity*, Game::LevelManager&)>;

	/** Make bombs explode */
	void bombExplosionLogic(Game::Entity *e, Game::LevelManager &lm);
	/** Make players drop bombs */
	void bombDeployLogic(Game::Entity *e, Game::LevelManager &lm);

	extern std::vector<GameLogicFunc> functions;
}

}
