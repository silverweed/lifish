#pragma once

#include <functional>

namespace lif {

class BufferedSpawner;
class LevelManager;
class Enemy;

void spawnInFreeTiles(lif::BufferedSpawner *spawner, lif::LevelManager& lm,
		int spawnedEnemyId, int nSpawned, int minDistFromPlayers,
		std::function<void(lif::Enemy*)> cb = nullptr);

}
