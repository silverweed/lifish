#include "Collider.hpp"
#include "CompoundCollider.hpp"
#include <sstream>
#include <algorithm>

using Game::Collider;

Collider::Collider(Game::Entity& owner, Game::Layers::Layer layer, const sf::Vector2i& size,
		const sf::Vector2f& offset, bool phantom)
	: Game::Component(owner)
	, phantom(phantom)
	, offset(offset)
	, size(size)
	, layer(layer)
{}

Collider::Collider(Game::Entity& owner, CollisionFunc onCollision,
		Game::Layers::Layer layer, const sf::Vector2i& size, 
		const sf::Vector2f& offset, bool phantom)
	: Game::Component(owner)
	, phantom(phantom)
	, offset(offset)
	, size(size)
	, layer(layer)
	, onCollision(onCollision)
{}

Collider::Collider(const Game::Collider& other)
	: Game::Component(other.owner)
	, phantom(other.phantom)
	, offset(other.offset)
	, size(other.size)
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

std::vector<std::weak_ptr<Collider>> Collider::getColliding() const { 
	return colliding; 
}

void Collider::addColliding(std::weak_ptr<Game::Collider> coll) {
	// avoid duplicates
	if (std::find_if(colliding.begin(), colliding.end(), [&coll] (std::weak_ptr<Game::Collider> oth) {
		return !oth.expired() && oth.lock().get() == coll.lock().get();
	}) == colliding.end()) {
		colliding.push_back(coll);
	}
}

bool Collider::contains(const Game::Collider& other) const {
	return other._contains(*this);
}

bool Collider::_contains(const Game::Collider& other) const {
	return getRect().intersects(other.getRect());
}

bool Collider::isColliding() const { 
	return atLimit || colliding.size() > 0; 
}

std::string Collider::toString() const {
	const auto rect = getRect();
	std::stringstream ss;
	ss << "[Collider @ (" << rect.left << ", " << rect.top << ") x (" << rect.width << ", " << rect.height << ")]";
	return ss.str();
}

void Collider::reset() {
	colliding.clear();
}

sf::Vector2f Collider::getPosition() const {
	return owner.getPosition() + offset;
}
