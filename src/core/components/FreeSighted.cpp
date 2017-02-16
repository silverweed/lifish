#include "FreeSighted.hpp"
#include "utils.hpp"
#include "LevelManager.hpp"

using lif::FreeSighted;

FreeSighted::FreeSighted(lif::Entity& owner, float visionRadius)
	: lif::Sighted(owner, visionRadius)
{
	keys.emplace_back(_getKey<FreeSighted>());
}

void FreeSighted::update() {
	lif::Component::update();
	if (entities == nullptr) return;

	seen.clear();

	const double sqrVR = visionRadius * lif::TILE_SIZE * visionRadius * lif::TILE_SIZE;
	entities->apply([this, sqrVR] (std::weak_ptr<lif::Entity> e) {
		if (e.expired())
			return;
		auto ptr = e.lock();
		// Don't see self
		if (ptr.get() == &owner)
			return;
		double dist = lif::sqrDistance(ptr->getPosition(), owner.getPosition());
		if (visionRadius > 0 && dist > sqrVR)
			return;
		// Only see living entities
		const auto killable = ptr->get<lif::Killable>();
		if (killable == nullptr || !killable->isKilled())
			seen.push_back(std::make_pair(e, dist));
	});
}
