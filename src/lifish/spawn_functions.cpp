#include "spawn_functions.hpp"
#include "LevelManager.hpp"
#include "BufferedSpawner.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "EnemyFactory.hpp"
#include "utils.hpp"
#include "collision_utils.hpp"
#include <random>
#include "game.hpp"

void lif::spawnInFreeTiles(lif::BufferedSpawner *spawner, lif::LevelManager& lm,
		int spawnedEnemyId, int nSpawned, int minDistFromPlayers,
		std::function<void(lif::Enemy*)> cb)
{
	auto viablePositions = lif::collision_utils::findFreeTiles(lm);

	// Don't spawn enemies near players
	viablePositions.erase(std::remove_if(viablePositions.begin(), viablePositions.end(),
		[&lm, minDistFromPlayers] (const auto& pos)
	{
		for (int i = 0; i < lif::MAX_PLAYERS; ++i) {
			const auto player = lm.getPlayer(i + 1);
			if (player != nullptr && lif::manhattanDistance(
					lif::tile(player->getPosition()), pos) <= minDistFromPlayers)
			{
				return true;
			}
		}
		return false;
	}), viablePositions.end());

	std::uniform_int_distribution<> dist(0, viablePositions.size() - 1);
	for (int i = 0; i < nSpawned; ++i) {
		const auto pos = sf::Vector2f(viablePositions[dist(lif::rng)] * lif::TILE_SIZE);
		auto enemy = lif::EnemyFactory::create(lm, spawnedEnemyId, pos);
		if (cb)
			cb(enemy.get());
		spawner->addSpawned(enemy.release());
	}
}
