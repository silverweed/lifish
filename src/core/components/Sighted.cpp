#include "Sighted.hpp"
#include "EntityGroup.hpp"

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

void Sighted::setEntityGroup(const Game::EntityGroup *eg) {
	entities = eg;
}
