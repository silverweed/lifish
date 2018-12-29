#pragma once

#include "Component.hpp"
#include "collision_layers.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>

namespace lif {

class Collider : public lif::Component {
public:
	using CollisionFunc = std::function<void(lif::Collider&)>;

protected:
	const bool phantom;

	/** All the Colliders which are colliding with this one */
	std::vector<std::weak_ptr<lif::Collider>> colliding;
	/** Whether this entity is at a level's boundary */
	bool atLimit = false;

	/** Offset relative to the owner's position */
	sf::Vector2f offset;
	sf::Vector2f size;
	bool forceAck = false;
	/** Collision layer */
	lif::c_layers::Layer layer;
	/** Optional callback to be called at every update */
	CollisionFunc onCollision;

public:
	COMP_NOT_UNIQUE

	explicit Collider(lif::Entity& owner,
			  lif::c_layers::Layer layer = lif::c_layers::DEFAULT,
			  const sf::Vector2f& size = sf::Vector2f(lif::TILE_SIZE, lif::TILE_SIZE),
			  const sf::Vector2f& offset = sf::Vector2f(0, 0),
			  bool phantom = false);

	explicit Collider(lif::Entity& owner,
			  CollisionFunc onCollision,
			  lif::c_layers::Layer layer = lif::c_layers::DEFAULT,
			  const sf::Vector2f& size = sf::Vector2f(lif::TILE_SIZE, lif::TILE_SIZE),
			  const sf::Vector2f& offset = sf::Vector2f(0, 0),
			  bool phantom = false);

	Collider(const lif::Collider& other);

	/** @return the list of Colliders colliding with this one */
	std::vector<std::weak_ptr<lif::Collider>> getColliding() const;
	/** Manually sets `coll` to be colliding with this collider */
	void addColliding(std::weak_ptr<lif::Collider> coll);
	/** Resets the list of colliding entities */
	void reset();

	/** @return the collision layer of this Collider */
	lif::c_layers::Layer getLayer() const { return layer; }
	/** Sets the collision layer of this Collider */
	void setLayer(lif::c_layers::Layer l) { layer = l; }

	sf::Vector2f getOffset() const { return offset; }
	void setOffset(const sf::Vector2f& off) { offset = off; }

	sf::Vector2f getSize() const { return size; }
	void setSize(const sf::Vector2f& sz) { size = sz; }

	/** If a Collider is phantom, it won't be automatically managed by EntityGroup
	 *  (and therefore CollisionDetector). A phantom collider may be useful if you
	 *  want to handle the collision logic yourself and you're only interested
	 *  in giving a collision layer to the collider.
	 */
	bool isPhantom() const { return phantom; }

	/** If a Collider requests "force ack", the CollisionDetector will be forced to
	 *  signal the collision to _both_ this collider and the one this is colliding with.
	 *  Useful to ensure this collision isn't missed by any of the parties involved,
	 *  e.g. for the Bullets, but may result in buggy collisions if misused.
	 *  Note that, in normal situations, the collisions are only signaled to the
	 *  _active_ colliding entity, while the _passive_ one only gets signaled if non-Moving.
	 *  It is only meaningful to override this behaviour in special cases, namely Bullets
	 *  and entities which self-destruct and apply effects when colliding.
	 */
	bool requestsForceAck() const { return forceAck; }
	void setForceAck(bool b) { forceAck = b; }

	/** @return whether this collider's layer collides with other's layer */
	bool collidesWith(const lif::Collider& other) const;

	/** @return whether this collider's layer is solid for other's layer */
	bool isSolidFor(const lif::Collider& other) const;

	/** @return whether this Collider is at the level boundary. This is set
	 *  externally by CollisionDetector.
	 */
	bool isAtLimit() const { return atLimit; }
	void setAtLimit(bool b) { atLimit = b; }

	/** @return whether this Collider intersects `other` */
	bool contains(const lif::Collider& other) const;

	/** @return whether this Collider is either "at limit" or has more than 0 colliding Colliders */
	bool isColliding() const;
	/** Like `isColliding`, but only account for colliders which are solid for this one */
	bool collidesWithSolid() const;
	bool collidesWithSolid(const sf::Vector2f& direction) const;

	/** @return the bounding box of this Collider */
	sf::FloatRect getRect() const;

	void update() override;

	std::string toString() const override;

	/** @return The actual position of this collider, including the offset */
	sf::Vector2f getPosition() const override;
};

}
