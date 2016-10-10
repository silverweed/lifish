#include "Spawning.hpp"
#include "Killable.hpp"
#include <exception>

using Game::Spawning;

Spawning::Spawning(Game::Entity& owner, SpawnFunction spawnFunction)
	: Game::Component(owner)
	, spawnFunction(spawnFunction)
{
	const auto klb = owner.get<Game::Killable>();
	if (klb == nullptr)
		throw std::invalid_argument("Default spawn function of Spawning is `spawn on death`, but "
				"owner of Spawning has no Killable!");
	spawnCondition = _spawnOnDeath(klb);
}

Spawning::Spawning(Game::Entity& owner, SpawnCondition spawnCondition, SpawnFunction spawnFunction)
	: Game::Component(owner)
	, spawnCondition(spawnCondition)
	, spawnFunction(spawnFunction)
{}

std::unique_ptr<Game::Entity> Spawning::spawn() {
	++_nSpawned;
	return std::unique_ptr<Game::Entity>(spawnFunction());
}

Spawning::SpawnCondition Spawning::_spawnOnDeath(const Game::Killable *klb) {
	return [klb] (const Game::Spawning& spw) {
		return spw.nSpawned() == 0 && klb->isKilled() && !klb->isKillInProgress();
	};
}
