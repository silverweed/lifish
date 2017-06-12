#pragma once

#include <memory>
#include <queue>
#include "Spawning.hpp"

namespace lif {

/**
 * This class is a generic Spawning component endued with a buffer.
 * Useful for complex behaviours where several entities may be spawned simultaneously
 * or in general when passing lambdas to the constructor of Spawning is unsuitable.
 */
class BufferedSpawner : public lif::Spawning {
protected:
	std::queue<std::unique_ptr<lif::Entity>> spawned;

public:
	explicit BufferedSpawner(lif::Entity& owner);

	bool shouldSpawn() const override;
	std::unique_ptr<lif::Entity> spawn() override;

	void addSpawned(lif::Entity *e);
	template<typename T>
	void addSpawned(std::unique_ptr<T>&& e) {
		addSpawned(e.release());
	}
};

}
