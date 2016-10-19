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
	entities->apply([this, sqrVR] (const Game::Entity *e) {
		double dist = Game::sqrDistance(e->getPosition(), owner.getPosition());
		if (sqrVR > 0 && dist > sqrVR) return;
		seen.push_back(std::make_pair(e, dist));
	});
}
