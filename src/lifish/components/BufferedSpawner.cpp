#include "BufferedSpawner.hpp"

using Game::BufferedSpawner;

BufferedSpawner::BufferedSpawner(Game::Entity& owner)
	: Game::Spawning(owner)
{}

std::unique_ptr<Game::Entity> BufferedSpawner::spawn() {
	std::unique_ptr<Game::Entity> result;
	if (!spawned.empty()) {
		result = std::unique_ptr<Game::Entity>(spawned.front().release());
		spawned.pop();
	}
	return result;
}

bool BufferedSpawner::shouldSpawn() const {
	return !spawned.empty();
}

void BufferedSpawner::_spawn(Game::Entity *e) {
	spawned.push(std::unique_ptr<Game::Entity>(e));
}
