#include "CompoundCollider.hpp"

using Game::CompoundCollider;
using Game::Collider;

CompoundCollider::CompoundCollider(Game::Entity& owner, Game::Layers::Layer layer,
		std::initializer_list<Game::Collider> clds)
	: Game::Collider(owner, layer)
	, colliders(clds)
{
	_calcBoundingRect();
}

CompoundCollider::CompoundCollider(const Game::CompoundCollider& other)
	: Game::Collider(other.owner, other.layer)
	, colliders(other.colliders)
{}

void CompoundCollider::_calcBoundingRect() {
	int minX = Game::TILE_SIZE * Game::LEVEL_WIDTH,
	    minY = Game::TILE_SIZE * Game::LEVEL_HEIGHT,
	    maxX = 0,
	    maxY = 0;
	for (const auto& c : colliders) {
		const auto crect = c.getRect();
		if (crect.left < minX)
			minX = crect.left;
		if (crect.left + crect.width > maxX)
			maxX = crect.left + crect.width;
		if (crect.top < minY)
			minY = crect.top;
		if (crect.top + crect.height > maxY)
			maxY = crect.top + crect.height;
	}

	boundingRect = sf::IntRect(minX, minY, maxX - minX, maxY - minY);
}

bool CompoundCollider::isColliding() const {
	for (const auto& c : colliders)
		if (c.isColliding()) 
			return true;
	return false;
}

bool CompoundCollider::contains(const Game::Collider& other) const {
	return _contains(other);
}

bool CompoundCollider::_contains(const Game::Collider& other) const {
	for (const auto& c : colliders)
		if (c.contains(other))
			return true;
	return false;
}

bool CompoundCollider::containsLoose(const Game::Collider& other) const {
	return boundingRect.intersects(other.getRect());
}

std::vector<Game::Collider> CompoundCollider::getSubCollidingWith(const Game::Collider& other) const {
	std::vector<Game::Collider> clds;
	clds.reserve(colliders.size());

	for (const auto& c : colliders)
		if (c.contains(other))
			clds.push_back(c);

	return clds;
}

sf::IntRect CompoundCollider::getRect() const {
	return boundingRect;
}

sf::Vector2i CompoundCollider::getSize() const {
	return sf::Vector2i(boundingRect.width, boundingRect.height);
}
