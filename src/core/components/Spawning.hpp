#pragma once

#include <memory>
#include <functional>
#include "Component.hpp"

namespace lif {

class Killable;

/**
 * An Entity with a `Spawning` component will spawn other Entities
 * during its lifetime (either once or several times).
 */
class Spawning : public lif::Component {

	/** This function SHOULD return false when `nSpawned` is greater than a threshold.
	 *  `*this` is passed for convenience to the function, so the state of
	 *  this component may be conveniently monitored.
	 */
	using SpawnCondition = std::function<bool(const lif::Spawning&)>;
	/** Returns the spawned entity (may return nullptr). */
	using SpawnFunction = std::function<lif::Entity*()>;

	SpawnCondition spawnCondition;
	SpawnFunction spawnFunction;

	unsigned _nSpawned = 0;


	/** Returns the condition for "spawn on death".
	 *  Since it's a common case, use as default constructor argument.
	 */
	SpawnCondition _spawnOnDeath(const lif::Killable*);

protected:
	/** Constructor used by child classes of Spawning, which typically override
	 *  `shouldSpawn` and `spawn` directly.
	 */
	explicit Spawning(lif::Entity& owner);

public:
	COMP_NOT_UNIQUE

	/** Constructs Spawning with the "spawn on death" condition. */
	explicit Spawning(lif::Entity& owner, SpawnFunction spawnFunction);
	explicit Spawning(lif::Entity& owner, SpawnCondition spawnCondition, SpawnFunction spawnFunction);

	unsigned nSpawned() const { return _nSpawned; }

	virtual bool shouldSpawn() const { return spawnCondition(*this); }
	/** @return the spawned entity */
	virtual std::unique_ptr<lif::Entity> spawn();
};

}
