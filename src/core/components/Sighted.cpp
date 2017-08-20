#include "Sighted.hpp"
#include "EntityGroup.hpp"

using lif::Sighted;

Sighted::Sighted(lif::Entity& owner, float visionRadius)
	: lif::Component(owner)
	, visionRadius(visionRadius)
{
	_declComponent<Sighted>();
}

void Sighted::setOpaque(std::initializer_list<lif::c_layers::Layer> layers, bool opaque) {
	for (auto layer : layers) {
		const auto shift = static_cast<unsigned>(layer);
		if (opaque)
			opaqueMask |= 1 << shift;
		else
			opaqueMask &= ~(1 << shift);
	}
}

bool Sighted::_isOpaque(lif::c_layers::Layer layer) const {
	return (opaqueMask >> static_cast<unsigned>(layer)) & 1;
}

void Sighted::setEntityGroup(const lif::EntityGroup *eg) {
	entities = eg;
}
