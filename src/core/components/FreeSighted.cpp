#include "FreeSighted.hpp"
#include "utils.hpp"
#include "LevelManager.hpp"

using Game::FreeSighted;

FreeSighted::FreeSighted(Game::Entity& owner, float visionRadius)
	: Game::Sighted(owner, visionRadius)
{}

void FreeSighted::update() {
	Game::Component::update();
	if (entities == nullptr) return;

	seen.clear();

	const double sqrVR = visionRadius * Game::TILE_SIZE * visionRadius * Game::TILE_SIZE;
	entities->apply([this, sqrVR] (std::weak_ptr<Game::Entity> e) {
		if (e.expired())
			return;
		auto ptr = e.lock();
		// Don't see self
		if (ptr.get() == &owner)
			return;
		double dist = Game::sqrDistance(ptr->getPosition(), owner.getPosition());
		if (visionRadius > 0 && dist > sqrVR)
			return;
		// Only see living entities
		const auto killable = ptr->get<Game::Killable>();
		if (killable == nullptr || !killable->isKilled())
			seen.push_back(std::make_pair(e, dist));
	});
}
