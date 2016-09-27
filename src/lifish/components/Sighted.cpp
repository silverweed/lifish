#include "Sighted.hpp"
#include "LevelManager.hpp"

using Game::Sighted;

Sighted::Sighted(Game::Entity& owner, float visionRadius)
	: Game::Component(owner)
	, visionRadius(visionRadius)
{}

void Sighted::setOpaque(std::initializer_list<Game::Layers::Layer> layers, bool opaque) {
	for (auto layer : layers) {
		const auto shift = static_cast<unsigned>(layer);
		if (opaque)
			opaqueMask |= 1 << shift;
		else
			opaqueMask &= ~(1 << shift); 
	}
}

bool Sighted::_isOpaque(Game::Layers::Layer layer) const {
	return (opaqueMask >> static_cast<unsigned>(layer)) & 1;
}

void Sighted::setLevelManager(const Game::LevelManager *_lm) {
	lm = _lm;
}

const Game::LevelManager* Sighted::getLevelManager() const {
	return lm;
}
