#include "Collider.hpp"

using Game::Collider;

Collider::Collider(Game::Entity *const owner, const sf::Vector2i& size, int layer)
	: Game::Component(owner)
	, size(size)
	, layer(layer)
{}

Collider::Collider(Game::Entity *const owner, const sf::Vector2i& size, 
		int layer, std::function<void(Game::Collider*)> onCollision)
	: Game::Component(owner)
	, size(size)
	, layer(layer)
	, onCollision(onCollision)
{}

void Collider::update() const {
	if (onCollision && isColliding())
		onCollision(colliding);
}
