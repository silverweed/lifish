#include "BufferedSpawner.hpp"

using lif::BufferedSpawner;

BufferedSpawner::BufferedSpawner(lif::Entity& owner)
	: lif::Spawning(owner)
{
	_declComponent<BufferedSpawner>();
}

std::unique_ptr<lif::Entity> BufferedSpawner::spawn() {
	std::unique_ptr<lif::Entity> result;
	if (!spawned.empty()) {
		result = std::unique_ptr<lif::Entity>(spawned.front().release());
		spawned.pop();
	}
	return result;
}

bool BufferedSpawner::shouldSpawn() const {
	return !spawned.empty();
}

void BufferedSpawner::addSpawned(lif::Entity *e) {
	spawned.push(std::unique_ptr<lif::Entity>(e));
}
