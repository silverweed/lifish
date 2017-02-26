#include "Spawning.hpp"
#include "Killable.hpp"
#include <exception>

using lif::Spawning;

Spawning::Spawning(lif::Entity& owner) : lif::Component(owner) {
	_declComponent<Spawning>();
}

Spawning::Spawning(lif::Entity& owner, SpawnFunction spawnFunction)
	: lif::Component(owner)
	, spawnFunction(spawnFunction)
{
	_declComponent<Spawning>();
	const auto klb = owner.get<lif::Killable>();
	if (klb == nullptr)
		throw std::invalid_argument("Default spawn function of Spawning is `spawn on death`, but "
				"owner of Spawning has no Killable!");
	spawnCondition = _spawnOnDeath(klb);
}

Spawning::Spawning(lif::Entity& owner, SpawnCondition spawnCondition, SpawnFunction spawnFunction)
	: lif::Component(owner)
	, spawnCondition(spawnCondition)
	, spawnFunction(spawnFunction)
{
	_declComponent<Spawning>();
}

std::unique_ptr<lif::Entity> Spawning::spawn() {
	++_nSpawned;
	return std::unique_ptr<lif::Entity>(spawnFunction());
}

Spawning::SpawnCondition Spawning::_spawnOnDeath(const lif::Killable *klb) {
	return [klb] (const lif::Spawning& spw) {
		return spw.nSpawned() == 0 && klb->isKilled() && !klb->isKillInProgress();
	};
}
