#pragma once

#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "collision_layers.hpp"

namespace Game {

class CollisionDetector;

class Collider : public Game::Component {
protected:
	using CollisionFunc = std::function<void(Game::Collider&)>;

	const bool phantom;

	/** All the Colliders which are colliding with this one */
	std::vector<std::weak_ptr<Game::Collider>> colliding;
	/** Whether this entity is at a level's boundary */
	bool atLimit = false;
	
	/** Offset relative to the owner's position */
	sf::Vector2f offset; 
	sf::Vector2i size;
	bool forceAck = false;
	/** Collision layer */
	Game::Layers::Layer layer;
	/** Optional callback to be called at every update */
	CollisionFunc onCollision;

public:
	COMP_NOT_UNIQUE

	explicit Collider(Game::Entity& owner, 
			  Game::Layers::Layer layer = Game::Layers::DEFAULT,
			  const sf::Vector2i& size = sf::Vector2i(Game::TILE_SIZE, Game::TILE_SIZE),
			  const sf::Vector2f& offset = sf::Vector2f(0, 0),
			  bool phantom = false);

	explicit Collider(Game::Entity& owner,
			  CollisionFunc onCollision,
			  Game::Layers::Layer layer = Game::Layers::DEFAULT,
			  const sf::Vector2i& size = sf::Vector2i(Game::TILE_SIZE, Game::TILE_SIZE), 
			  const sf::Vector2f& offset = sf::Vector2f(0, 0),
			  bool phantom = false);
	
	Collider(const Game::Collider& other);

	/** @return the list of Colliders colliding with this one */
	std::vector<std::weak_ptr<Game::Collider>> getColliding() const;
	/** Manually sets `coll` to be colliding with this collider */
	void addColliding(std::weak_ptr<Game::Collider> coll);
	/** Resets the list of colliding entities */
	void reset();

	/** @return the collision layer of this Collider */
	Game::Layers::Layer getLayer() const { return layer; }
	/** Sets the collision layer of this Collider */
	void setLayer(Game::Layers::Layer l) { layer = l; }

	sf::Vector2f getOffset() const { return offset; }
	void setOffset(const sf::Vector2f& off) { offset = off; }

	virtual sf::Vector2i getSize() const { return size; }
	void setSize(const sf::Vector2i& sz) { size = sz; }

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
	bool collidesWith(const Game::Collider& other) const;

	/** @return whether this collider's layer is solid for other's layer */
	bool isSolidFor(const Game::Collider& other) const;

	/** @return whether this Collider is at the level boundary. This is set
	 *  externally by CollisionDetector.
	 */
	bool isAtLimit() const { return atLimit; }
	void setAtLimit(bool b) { atLimit = b; }

	/** @return whether this Collider intersects `other` */
	virtual bool contains(const Game::Collider& other) const;

	/** @return whether this Collider is either "at limit" or has more than 0 colliding Colliders */
	virtual bool isColliding() const;
	/** Like `isColliding`, but only account for colliders which are solid for this one */
	virtual bool collidesWithSolid() const;

	/** @return the bounding box of this Collider */
	virtual sf::IntRect getRect() const;

	virtual void update() override;

	std::string toString() const override;

	/** @return The actual position of this collider, including the offset */
	sf::Vector2f getPosition() const override;
};

}
