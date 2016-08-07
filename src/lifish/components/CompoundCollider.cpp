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

bool CompoundCollider::contains(const CompoundCollider& other) const {
	if (Collider::contains(static_cast<Collider>(other))) return true;
	for (const auto& c : colliders)
		if (c.contains(static_cast<Collider>(other))) return true;
	for (const auto& oc : other.colliders) {
		if (Collider::contains(oc)) return true;
		for (const auto& c : colliders)
			if (c.contains(oc)) return true;
	}
	return false;
}

bool CompoundCollider::contains(const Collider& other) const {
	if (Collider::contains(other)) return true;
	for (const auto& c : colliders)
		if (c.contains(other)) return true;
	return false;
}

sf::IntRect CompoundCollider::getRect() const {
	auto rect = Collider::getRect();

	for (const auto& c : colliders) {
		auto crect = c.getRect();
		if (crect.left < rect.left) {
			rect.left = crect.left;
			rect.width += crect.left;
		}
		if (crect.top < rect.top) {
			rect.top = crect.top;
			rect.height += crect.top;
		}
	}
	return rect;
}
