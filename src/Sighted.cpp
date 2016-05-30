#include "Sighted.hpp"
#include "LevelManager.hpp"

using Game::Sighted;

Sighted::Sighted(Game::Entity *const owner, short visionRadius)
	: Game::Component(owner)
	, visionRadius(visionRadius)
{
	seen.fill(std::make_pair(nullptr, 0));
}

void Sighted::update() {
	if (lm == nullptr) return;
	// TODO
}
