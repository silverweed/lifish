#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "game.hpp"
#include "collision_layers.hpp"

namespace Game {

class CollisionDetector;

class Collider : public Game::Component {
	friend class Game::CollisionDetector;

	using CollisionFunc = std::function<void(Game::Collider&)>;

	/** All the Colliders which are colliding with this one */
	std::vector<std::reference_wrapper<Game::Collider>> colliding;
	/** Whether this entity is at a level's boundary */
	bool atLimit = false;
	
	/** Offset relative to the owner's position */
	const sf::Vector2f offset; 
	const sf::Vector2i size;
	/** If a Collider is phantom, it won't be automatically managed by EntityGroup
	 *  (and therefore CollisionDetector). A phantom collider may be useful if you
	 *  want to handle the collision logic yourself and you're only interested
	 *  in giving a collision layer to the collider.
	 */
	const bool phantom;
	/** Collision layer */
	Game::Layers::Layer layer;
	/** Optional callback to be called at every update */
	CollisionFunc onCollision;

public:
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

	/** @return the list of Colliders colliding with this one */
	std::vector<std::reference_wrapper<Game::Collider>> getColliding() const { return colliding; }
	/** Manually sets `coll` to be colliding with this collider */
	void addColliding(Game::Collider& coll) { colliding.push_back(coll); }

	/** @return the collision layer of this Collider */
	Game::Layers::Layer getLayer() const { return layer; }
	/** Sets the collision layer of this Collider */
	void setLayer(Game::Layers::Layer l) { layer = l; }

	bool isPhantom() const { return phantom; }

	/** @return whether this collider's layer collides with other's layer */
	bool collidesWith(const Game::Collider& other) const;
	/** @return whether this collider's layer is solid for other's layer */
	bool isSolidFor(const Game::Collider& other) const;

	/** @return whether this Collider is at the level boundary. This is set
	 *  externally by CollisionDetector.
	 */
	bool isAtLimit() const { return atLimit; }

	virtual bool isColliding() const { return atLimit || colliding.size() > 0; }
	/** Like `isColliding`, but only account for colliders which are solid for this one */
	virtual bool collidesWithSolid() const;
	virtual bool contains(const Game::Collider& other) const {
		return getRect().intersects(other.getRect());
	}
	/** @return the bounding box of this Collider */
	virtual sf::IntRect getRect() const;

	virtual void update() override;
};

}
