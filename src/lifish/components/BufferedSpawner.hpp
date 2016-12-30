#pragma once

#include <memory>
#include <queue>
#include "Spawning.hpp"

namespace Game {

/**
 * This class is a generic Spawning component endued with a buffer.
 * Useful for complex behaviours where several entities may be spawned simultaneously
 * or in general when passing lambdas to the constructor of Spawning is unsuitable.
 */
class BufferedSpawner : public Game::Spawning {
protected:
	std::queue<std::unique_ptr<Game::Entity>> spawned;

public:
	explicit BufferedSpawner(Game::Entity& owner);

	bool shouldSpawn() const override;
	std::unique_ptr<Game::Entity> spawn() override;

	void addSpawned(Game::Entity *e);
};

}
