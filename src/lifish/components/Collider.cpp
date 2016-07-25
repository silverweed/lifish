#include "Collider.hpp"

using Game::Collider;

Collider::Collider(Game::Entity& owner, Game::Layers::Layer layer, const sf::Vector2i& size, bool phantom)
	: Game::Component(owner)
	, size(size)
	, phantom(phantom)
	, layer(layer)
{}

Collider::Collider(Game::Entity& owner, CollisionFunc onCollision,
		Game::Layers::Layer layer, const sf::Vector2i& size, bool phantom)
	: Game::Component(owner)
	, size(size)
	, phantom(phantom)
	, layer(layer)
	, onCollision(onCollision)
{}

void Collider::update() {
	Game::Component::update();
	
	if (onCollision)
		for (auto& cld : colliding)
			onCollision(cld);
}

bool Collider::collidesWithSolid() const {
	if (atLimit) return true;
	for (const auto& c : colliding)
		if (c.get().isSolidFor(*this))
			return true;
	return false;
}
