#include "FreeSighted.hpp"
#include "utils.hpp"
#include "LevelManager.hpp"

using lif::FreeSighted;

FreeSighted::FreeSighted(lif::Entity& owner, float visionRadius)
	: lif::Sighted(owner, visionRadius)
{
	_declComponent<FreeSighted>();
}

void FreeSighted::update() {
	lif::Component::update();
	if (entities == nullptr) return;

	seen.clear();

	const auto sqrVR = visionRadius * lif::TILE_SIZE * visionRadius * lif::TILE_SIZE;
	entities->apply([this, sqrVR] (lif::Entity& e) {
		// Don't see self
		if (&e == &owner)
			return;
		const auto dist = lif::sqrDistance(e.getPosition(), owner.getPosition());
		if (visionRadius > 0 && dist > sqrVR)
			return;
		// Only see living entities
		const auto killable = e.get<lif::Killable>();
		if (killable == nullptr || !killable->isKilled())
			seen.emplace_back(&e, dist);
	});
}
