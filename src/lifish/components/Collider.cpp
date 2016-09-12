#include "Collider.hpp"

using Game::Collider;

Collider::Collider(Game::Entity& owner, Game::Layers::Layer layer, const sf::Vector2i& size,
		const sf::Vector2f& offset, bool phantom)
	: Game::Component(owner)
	, offset(offset)
	, size(size)
	, phantom(phantom)
	, layer(layer)
{}

Collider::Collider(Game::Entity& owner, CollisionFunc onCollision,
		Game::Layers::Layer layer, const sf::Vector2i& size, 
		const sf::Vector2f& offset, bool phantom)
	: Game::Component(owner)
	, offset(offset)
	, size(size)
	, phantom(phantom)
	, layer(layer)
	, onCollision(onCollision)
{}

void Collider::update() {
	Game::Component::update();
	
	if (onCollision)
		for (auto cld : colliding)
			if (!cld.expired())
				onCollision(*cld.lock().get());
}

bool Collider::collidesWithSolid() const {
	if (atLimit) return true;
	for (auto c : colliding)
		if (!c.expired() && c.lock().get()->isSolidFor(*this))
			return true;
	return false;
}

sf::IntRect Collider::getRect() const {
	const auto pos = owner.getPosition() + offset;
	return sf::IntRect(pos.x, pos.y, size.x, size.y);
}

bool Collider::collidesWith(const Game::Collider& other) const {
	return Game::Layers::collide[layer][other.layer];
}

bool Collider::isSolidFor(const Game::Collider& other) const {
	return Game::Layers::solid[layer][other.layer];
}

