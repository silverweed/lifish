#include "Collider.hpp"
#include <sstream>
#include <algorithm>

using lif::Collider;

Collider::Collider(lif::Entity& owner, lif::c_layers::Layer layer, const sf::Vector2f& size,
		const sf::Vector2f& offset, bool phantom)
	: lif::Component(owner)
	, phantom(phantom)
	, offset(offset)
	, size(size)
	, layer(layer)
{
	_declComponent<Collider>();
}

Collider::Collider(lif::Entity& owner, CollisionFunc onCollision,
		lif::c_layers::Layer layer, const sf::Vector2f& size, 
		const sf::Vector2f& offset, bool phantom)
	: lif::Component(owner)
	, phantom(phantom)
	, offset(offset)
	, size(size)
	, layer(layer)
	, onCollision(onCollision)
{
	_declComponent<Collider>();
}

Collider::Collider(const lif::Collider& other)
	: lif::Component(other.owner)
	, phantom(other.phantom)
	, offset(other.offset)
	, size(other.size)
	, layer(other.layer)
{
	_declComponent<Collider>();
}


void Collider::update() {
	lif::Component::update();
	
	if (onCollision)
		for (auto cld : colliding)
			if (!cld.expired())
				onCollision(*cld.lock());
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

bool Collider::collidesWith(const lif::Collider& other) const {
	return lif::c_layers::collide[layer][other.layer];
}

bool Collider::isSolidFor(const lif::Collider& other) const {
	return lif::c_layers::solid[layer][other.layer];
}

std::vector<std::weak_ptr<Collider>> Collider::getColliding() const { 
	return colliding; 
}

void Collider::addColliding(std::weak_ptr<lif::Collider> coll) {
	// avoid duplicates
	if (std::find_if(colliding.begin(), colliding.end(), [&coll] (std::weak_ptr<lif::Collider> oth) {
		return !oth.expired() && oth.lock().get() == coll.lock().get();
	}) == colliding.end()) {
		colliding.push_back(coll);
	}
}

bool Collider::contains(const lif::Collider& other) const {
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
