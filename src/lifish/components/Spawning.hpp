#pragma once

#include <forward_list>
#include "Component.hpp"

namespace Game {

/**
 * An Entity with a `Spawning` component will spawn another Entity
 * during its lifetime (either once or several times).
 */
class Spawning : public Game::Component {

	using SpawnCondition = std::function<bool(std::forward_list<Game::Entity*>&)>;

	SpawnCondition spawnCondition;
	std::forward_list<Game::Entity*> spawned;
	bool _hasSpawned = false;

public:

	/** `spawnCondition` is a function which returns whether it has spawned something or not
	 *  and fills the given list with the spawned Entities, if any.
	 */
	explicit Spawning(Game::Entity& owner, SpawnCondition spawnCondition)
		: Game::Component(owner)
		, spawnCondition(spawnCondition)
	{}

	bool hasSpawned() const { return _hasSpawned; }

	/** Returns the spawned entities and resets the internal spawned list */
	std::forward_list<Game::Entity*> spawn() {
		auto lst = spawned;
		spawned.clear();
		_hasSpawned = false;
		return lst;
	}


	void update() override {
		_hasSpawned |= spawnCondition(spawned);
	}
};

}
