#include "Collider.hpp"

using Game::Collider;

Collider::Collider(Game::Entity *const owner, Game::Layers::Layer layer, const sf::Vector2i& size)
	: Game::Component(owner)
	, size(size)
	, layer(layer)
{}

Collider::Collider(Game::Entity *const owner, std::function<void(Game::Collider*)> onCollision,
		Game::Layers::Layer layer, const sf::Vector2i& size)
	: Game::Component(owner)
	, size(size)
	, layer(layer)
	, onCollision(onCollision)
{}

void Collider::update() {
	if (onCollision && isColliding())
		onCollision(colliding);
}
