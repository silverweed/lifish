#pragma once

#include <memory>
#include "Component.hpp"

namespace Game {

class Killable;

/**
 * An Entity with a `Spawning` component will spawn other Entities
 * during its lifetime (either once or several times).
 */
class Spawning : public Game::Component {

	/** This function SHOULD return false when `nSpawned` is greater than a threshold.
	 *  `*this` is passed for convenience to the function, so the state of
	 *  this component may be conveniently monitored.
	 */
	using SpawnCondition = std::function<bool(const Game::Spawning&)>;
	/** Returns the spawned entity (may return nullptr). */
	using SpawnFunction = std::function<Game::Entity*()>;

	SpawnCondition spawnCondition;
	SpawnFunction spawnFunction;

	unsigned short _nSpawned = 0;


	/** Returns the condition for "spawn on death". 
	 *  Since it's a common case, use as default constructor argument. 
	 */
	SpawnCondition _spawnOnDeath(const Game::Killable*);

public:
	/** Constructs Spawning with the "spawn on death" condition. */
	explicit Spawning(Game::Entity& owner, SpawnFunction spawnFunction);
	explicit Spawning(Game::Entity& owner, SpawnCondition spawnCondition, SpawnFunction spawnFunction);

	bool shouldSpawn() const { return spawnCondition(*this); }

	/** Returns the spawned entity */
	std::unique_ptr<Game::Entity> spawn();

	unsigned short nSpawned() const { return _nSpawned; }
};

}
