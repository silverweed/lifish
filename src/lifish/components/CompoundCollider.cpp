#include "CompoundCollider.hpp"

using Game::CompoundCollider;
using Game::Collider;

void CompoundCollider::update() {
	// Don't call Collider::update, since a CompoundCollider
	// cannot have onCollision callbacks
	Game::Component::update();
}

bool CompoundCollider::isColliding() const {
	if (Collider::isColliding()) return true;
	for (const auto& c : colliders)
		if (c.isColliding()) return true;
	return false;
}

bool CompoundCollider::contains(const Collider& other) const {
	if (Collider::getRect().intersects(other.getRect())) return true;
	for (const auto& c : colliders)
		if (c.contains(other)) return true;
	return false;
}

sf::IntRect CompoundCollider::getRect() const {
	auto rect = Collider::getRect();

	for (const auto& c : colliders) {
		auto crect = c.getRect();
		if (crect.left < rect.left) {
			rect.width += rect.left - crect.left;
			rect.left = crect.left;
		}
		if (crect.top < rect.top) {
			rect.height += rect.top - crect.top;
			rect.top = crect.top;
		}
		if (crect.width > rect.width)
			rect.width = crect.width;
		if (crect.height > rect.height)
			rect.height = crect.height;
	}
	return rect;
}
