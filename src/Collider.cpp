#include "Collider.hpp"

using Game::Collider;

Collider::Collider(Game::Entity *const owner, Game::Layers::Layer layer, const sf::Vector2i& size, bool phantom)
	: Game::Component(owner)
	, size(size)
	, phantom(phantom)
	, layer(layer)
{}

Collider::Collider(Game::Entity *const owner, std::function<void(Game::Collider*)> onCollision,
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
		for (auto cld : colliding)
			onCollision(cld);
}
